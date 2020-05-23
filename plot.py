import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import save_login

conn = save_login.db_connecting()
cur = conn.cursor()
cur.execute("SELECT c FROM test2")
data = cur.fetchall()
def data_gen():
	for cnt in range(len(data)):
		yield cnt, data[cnt][0]



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
