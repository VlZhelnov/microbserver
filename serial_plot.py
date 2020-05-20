import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time

ser = serial.Serial("/dev/ttyUSB0", 9600)

def data_gen():
	for cnt in range(10000):
		time.sleep(0.003)
		t = cnt
		z =int(ser.read().hex(),16)*256 + int(ser.read().hex(), 16)
		print (t, np.float64(z))
		yield t, np.float64(z)



def init():
	ax.set_ylim(0, 1024) 
	ax.set_xlim(0, 10)
	del xdata[:]
	del ydata[:]
	line.set_data(xdata, ydata)
	return line,

fig, ax = plt.subplots()
line, = ax.plot([],[], lw=2)
ax.grid()
xdata, ydata = [], []

def run(data):
	t, y = data
	xdata.append(t)
	ydata.append(y)
	xmin,xmax = ax.get_xlim()
	if t >= xmax:
		ax.set_xlim(xmin,2*xmax)
		ax.figure.canvas.draw()
	line.set_data(xdata, ydata)
	return line,

ani = animation.FuncAnimation(fig, run, data_gen, blit=False, interval=10,repeat = False, init_func = init)

plt.show() 
