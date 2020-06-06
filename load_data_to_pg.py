import serial
import datetime
import save_login
from psycopg2 import extras

conn = save_login.db_connecting()
cur = conn.cursor(cursor_factory=extras.DictCursor)
if cur:
	while(True):	
		cur.execute("SELECT * FROM data_viewer_microrequest WHERE status='adopted'")
		tasks = cur.fetchall()
		if tasks:
			request_for_mk ="S{:0>5}{:0>5}".format(tasks[0]['quantity'], tasks[0]['delay']).encode()
			ser = serial.Serial("/dev/ttyUSB0", 9600)
			if ser:
				try:
					cur.execute("""UPDATE data_viewer_microrequest SET data_accept=%s, status=%s WHERE id=%s""",
														(datetime.datetime.now()-datetime.timedelta(hours=3), 'processing', tasks[0]['id']))
					conn.commit()
					ser.write(request_for_mk)
					print("out: " + ser.readline().decode().strip())
					for row in range(tasks[0]['quantity']):
						row_data = ser.readline().decode().strip().split(';')
						cur.execute("""INSERT INTO data_viewer_entry (interval, temperature, 
																	  illumination, microrequest_id) 
								   		VALUES(%s * interval '1 seconds', %s, %s,%s)""",
								   		(row_data[0],row_data[1],row_data[2], tasks[0]['id']))
						conn.commit()
					cur.execute("UPDATE data_viewer_microrequest SET data_completed=%s, status=%s WHERE id=%s",
														 (datetime.datetime.now()-datetime.timedelta(hours=3),'completed', tasks[0]['id']))
					conn.commit()
				except Exception as e:
					print(e)
					cur.execute("UPDATE data_viewer_microrequest SET data_completed=%s, status=%s WHERE id=%s",
														 (datetime.datetime.now()-datetime.timedelta(hours=3), 'error', tasks[0]['id']))
					conn.commit()
					print("Serial connect foult")
					cur.close()
					conn.close()
					exit(1)

