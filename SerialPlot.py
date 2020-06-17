import matplotlib.pyplot as plt
import serial

ser = serial.Serial("/dev/ttyUSB0", 9600)
request_for_mk ="G{:0>5}{:0>5}".format(1000,1).encode()
ser.write(request_for_mk)

tdata, adata, bdata, cdata, vdata = [], [], [], [], []

while(len(tdata)!=10):
    a = ser.readline().decode().strip().split(';')
    print(a)
    tdata.append(int(a[0]))
    adata.append(int(a[1]))
    bdata.append(int(a[2]))




plt.plot(tdata, adata, "r-",  label = "Температура")
plt.plot(tdata, bdata, "y-", dashes =[5,1], label = "Освещение")

plt.ylabel("О3")
plt.xlabel("Время, сек")
plt.legend()
plt.grid()
plt.show() 
