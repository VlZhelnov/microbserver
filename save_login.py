import argparse
import getpass
import keyring
import subprocess
import psycopg2	

systemname = "dbvmjkabf4cnq6"
username = "diuyoekvksyxbg"

def parse_arguments():
	parser = argparse.ArgumentParser()
	parser.add_argument("-n", "--newpass", required=False, help="Set new password", action="store_true")
	return parser.parse_args()

def db_connecting():
	db_name = "dbvmjkabf4cnq6"
	db_host = "ec2-3-91-139-25.compute-1.amazonaws.com"
	passwd = keyring.get_password(systemname, username)
	try:
		conn = psycopg2.connect(database = db_name,
								user = username,
								password=passwd,
								host = db_host,
								port = "5432")
		return conn
	except:
		return None

if __name__ == '__main__':
	args = parse_arguments()

	if args.newpass:
		password = getpass.getpass(prompt = "Enter secret password:")
		try:
			keyring.set_password(systemname, username, password)
		except Exception as error:
			print("Error: {}".format(error))

	db_connecting()
