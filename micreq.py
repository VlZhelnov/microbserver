import argparse
import getpass
import keyring
import psycopg2 
import serial
import pandas as pd
import matplotlib.pyplot as plt

SQL_NEW_REQUEST = """INSERT INTO microrequest (title, delay, quantity) VALUES  (%s, %s, %s);"""
SQL_INSERT_ENTRY = """INSERT INTO entry(elapsed_time, temperature, illumination, microrequest_id) VALUES (%s, %s, %s, %s);"""
SQL_START_REQUEST = """UPDATE microrequest SET (data_accept, status) = (current_timestamp, 'processing') WHERE id = %s;"""
SQL_CLEAR_REQUEST = """UPDATE microrequest SET (data_completed, status) = (current_timestamp, 'error') WHERE status = 'processing';"""
SQL_DONE_REQUEST =  """UPDATE microrequest SET (status, data_completed) = ('completed', current_timestamp) WHERE id = %s;"""
SQL_ERROR_REQUEST = """UPDATE microrequest SET (title, status, data_completed) = (%s, 'error', current_timestamp) WHERE id = %s;"""
SQL_GET_ENTRIES = """SELECT elapsed_time, temperature, illumination from entry WHERE microrequest_id = %s;"""
SQL_GET_REQUEST_ADOPTED = """SELECT id, title, delay, quantity  FROM microrequest WHERE status='adopted';"""
SQL_GET_REQUEST = """SELECT title, delay, quantity FROM microrequest WHERE id = %s ;"""
SQL_SELECT_REQUEST = """SELECT * FROM microrequest;"""
SQL_DELETE_REQUEST = """DELETE FROM microrequest WHERE id = %s""";


def parse_arguments():
    parser = argparse.ArgumentParser(description = "Connect to PostgreSQL and send request")
    parser.add_argument("user", help="Username for database")
    parser.add_argument("dbname", help="Name database")
    parser.add_argument("-host", default='localhost', help="default localhost")
    parser.add_argument("-p", "--port", default='5432', help="default 5432")
    parser.add_argument("-n", "--newpass", help="Set new password in keyring", action="store_true")
    parser.add_argument("--request", metavar=("title", "delay", "quantity"), type=str, nargs=3, help='vector request')
    parser.add_argument("--plot", metavar=("request_id"), type=str, nargs=1, help='Number request')
    parser.add_argument("-s", "--server", help="Start server", action="store_true")
    parser.add_argument("-d", "--delete", metavar=("request_id"), type=str, nargs=1, help='Number request')
    parser.add_argument("-i", "--info", help="Table request", action="store_true")
    return parser.parse_args()

def new_request(connection,cursor, title, delay, quantity):
    cursor.execute(SQL_NEW_REQUEST,(title, delay, quantity))
    connection.commit()
    print("Create new request")

def init_micro(seral): 
    seral.write("G0000300001".encode())
    for i in range(4):
        seral.readline()

def server(connection, cursor):
    cursor.execute(SQL_CLEAR_REQUEST)
    connection.commit()
    while(True):
        cursor.execute(SQL_GET_REQUEST_ADOPTED)
        tasks = cursor.fetchall()
        if tasks:
            microreq_id = str(tasks[0][0])
            try:
                with serial.Serial("/dev/ttyUSB0", 9600) as seral:
                    init_micro(seral)
                    request_for_mk ="G{:0>5}{:0>5}".format(tasks[0][3], tasks[0][2]).encode()
                    cursor.execute(SQL_START_REQUEST, (microreq_id,))
                    connection.commit()
                    seral.write(request_for_mk)
                    print("out: " + seral.readline().decode().strip())
                    for row in range(tasks[0][3]):
                        row_data = seral.readline().decode().strip().split(';')
                        cursor.execute(SQL_INSERT_ENTRY,(*row_data, microreq_id))
                        connection.commit()
                    cursor.execute(SQL_DONE_REQUEST, (microreq_id,))
                    connection.commit()
                    print("DONE")
            except Exception as e:
                cursor.execute(SQL_ERROR_REQUEST,(" ".join([tasks[0][1], "ERROR:", str(e)]), microreq_id))
                connection.commit()
                print(e)

def info(cursor):
    cursor.execute(SQL_SELECT_REQUEST)
    columns = [row[0] for row in cursor.description]
    all_requests = cursor.fetchall()
    df = pd.DataFrame(all_requests, columns=columns).set_index("id", drop=True)
    return df



def plot(cursor, microrequest_id):
    cursor.execute(SQL_GET_ENTRIES, (microrequest_id,))
    columns = [row[0] for row in cursor.description]
    all_entries = cursor.fetchall()
    df = pd.DataFrame(all_entries, columns=columns)

    cursor.execute(SQL_GET_REQUEST,(microrequest_id,))
    request = cursor.fetchone()

    fig, ax1 = plt.subplots()
    color = 'tab:red'
    ax1.set_xlabel('time (s)')
    ax1.set_title("{}\nDelay: {}   Quantity: {}".format(*request))
    ax1.set_ylabel('Temperature ($^\circ$C)', color=color)
    ax1.plot(df["elapsed_time"], df["temperature"], color=color)
    ax1.tick_params(axis='y', labelcolor=color)
    color = 'tab:purple'
    ax2 = ax1.twinx() 
    ax2.set_ylabel('Lighting, %', color=color)
    ax2.plot(df["elapsed_time"], df["illumination"], color=color)
    ax2.tick_params(axis='y',   labelcolor=color)
    fig.tight_layout()
    plt.show()

def delete(connection, cursor, microrequest_id):
    cursor.execute(SQL_DELETE_REQUEST, (microrequest_id,))
    connection.commit()


if __name__ == '__main__':
    args = parse_arguments()

    if args.newpass:
        password = getpass.getpass(prompt = "Enter secret password:")
        try:
            keyring.set_password(args.dbname, args.user, password)
        except Exception as error:
            print("Error: {}".format(error))

    passwd = keyring.get_password(args.dbname, args.user)

    if not passwd:
        print("Error: Password not found.Use --newpass for set new password")
        exit(1)
    login_db = "host={} port={} dbname={} user={} password={}"
    with psycopg2.connect(login_db.format(args.host, args.port, args.dbname,
                                          args.user, passwd)) as connection:
        with connection.cursor() as cursor:
            if args.request: new_request(connection, cursor, *args.request)
            if args.server: server(connection, cursor)
            if args.info: print(info(cursor))
            if args.plot: plot(cursor,args.plot[0])
            if args.delete: delete(connection, cursor, args.delete[0])
    exit(0)

