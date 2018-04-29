#!/usr/bin/env python  
import rospy
import math
import tf
import geometry_msgs.msg
import tf2_ros
import serial
import re
import struct
from Tkinter import *

global w1 
global w2 
global w3 
global w4 

rospy.init_node('tfff')

br = tf2_ros.TransformBroadcaster()
t = geometry_msgs.msg.TransformStamped()
pub = rospy.Publisher('rov/sensors', geometry_msgs.msg.Pose, queue_size=10)
pose = geometry_msgs.msg.Pose()

def sel():
	w1.set(1000)
	w2.set(1000)
	w3.set(1000)
	w4.set(90)
	pass
def call(event):
	temp = struct.pack("iiii", w1.get(), w2.get(), w3.get(), w4.get())
	comd = b'\x0f'+temp+b'\n'
	port.write(comd)
	xxx = port.read()
	print(xxx)



if __name__ == '__main__':

	master = Tk()
	w1 = Scale(master, from_=1000, to=2000, length=1000, label='motor1', orient=HORIZONTAL)
	w1.set(1000)
	w1.pack()

	w2 = Scale(master, from_=1000, to=2000, length=1000, label='motor2', orient=HORIZONTAL)
	w2.set(1000)
	w2.pack()
	w3 = Scale(master, from_=1000, to=2000, length=1000, label='motor3', orient=HORIZONTAL)
	w3.set(1000)
	w3.pack()
	w4 = Scale(master, from_=0, to=180, length=200, label='motor4', orient=HORIZONTAL)
	w4.set(90)
	w4.pack(anchor='w')
	button = Button(master, text="Reset", command=sel)
	button.pack(anchor=CENTER)

	rospy.Timer(rospy.Duration(0.1), call)
	with serial.Serial('/dev/ttyACM0', 115200) as port:
		mainloop()


