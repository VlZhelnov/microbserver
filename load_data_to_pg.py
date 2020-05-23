import serial
import time
import save_login

c = 5000
d = 1 
ser = serial.Serial("/dev/ttyUSB0", 9600)
conn = save_login.db_connecting()
cur = conn.cursor()
if cur:
	ser.write("S{:0>5}{:0>5}".format(c,d).encode())
	print(ser.readline().decode().strip())
	for z in range(c):
		x = ser.readline().decode().strip().split(';')
		cur.execute("INSERT INTO test2 (a, b, c) VALUES(%s * interval '1 seconds', %s, %s)", x)
		conn.commit()
	cur.close()
	conn.close()
