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
global d

rospy.init_node('tfff')

br = tf2_ros.TransformBroadcaster()
t = geometry_msgs.msg.TransformStamped()
pub = rospy.Publisher('rov/sensors', geometry_msgs.msg.Pose, queue_size=10)
pose = geometry_msgs.msg.Pose()

def sel():
	global d	
	w1.set(1460)
	w2.set(1460)
	w3.set(1460)
	w4.set(90)
	w5.set(90)
	d = 0;
	pass

def dep():
	global d
	if d == 0:
		d = 1
	else:
		d = 0

def call(event):
	temp = struct.pack("iiiiii", w1.get(), w2.get(), w3.get(), w4.get(), w5.get()*255/2042, d)
	comd = b'\x0f'+temp+b'\n'
	port.write(comd)
	while True:
		xx = port.read()
		if xx == b'\xf0':
			xxx = port.read(36)
			xx = port.read()
			break
	state = struct.unpack('fffffffff', xxx)
	print(':'.join(x.encode('hex') for x in xxx))
	print ('%06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f'%state)
	print(d)
	t.header.stamp = rospy.Time.now()
	t.header.frame_id = 'map'
	t.child_frame_id = 'rov'
	q = tf.transformations.quaternion_from_euler(state[2]/180*3.1415926, state[1]/180*3.1415926, -state[0]/180*3.1415926)
	t.transform.rotation.x = q[0]
	t.transform.rotation.y = q[1]
	t.transform.rotation.z = q[2]
	t.transform.rotation.w = q[3]
	pose.orientation.x = state[2]
	pose.orientation.y = state[1]
	pose.orientation.z = -state[0]
	pose.orientation.w = state[3]
	t.transform.translation.x = 0
	t.transform.translation.y = 0
	t.transform.translation.z = -state[4]
	pose.position.z = -state[4]
	br.sendTransform(t)
	pub.publish(pose)



if __name__ == '__main__':

	master = Tk()
	
	d = 0 
	
	w1 = Scale(master, from_=1000, to=2000, length=1000, label='motor1', orient=HORIZONTAL)
	w1.set(1460)
	w1.pack()

	w2 = Scale(master, from_=1000, to=2000, length=1000, label='motor2', orient=HORIZONTAL)
	w2.set(1460)
	w2.pack()
	w3 = Scale(master, from_=1000, to=2000, length=1000, label='motor3', orient=HORIZONTAL)
	w3.set(1460)
	w3.pack()
	w4 = Scale(master, from_=0, to=180, length=200, label='motor4', orient=HORIZONTAL)
	w4.set(90)
	w4.pack(anchor='w')
	
        w5 = Scale(master, from_=0, to=255, length=200, label='light', orient=HORIZONTAL)
	w5.set(0)
	w5.pack(anchor='w')

	button = Button(master, text="Reset", command=sel)
	button.pack(anchor=CENTER)
        button.focus_set()  
	button1 = Button(master, text="Depth", command=dep)
	button1.pack(anchor=NE)
	rospy.Timer(rospy.Duration(0.1), call)
	with serial.Serial('/dev/ttyUSB0', 115200) as port:
		mainloop()











	# with serial.Serial('/dev/ttyUSB0', 115200) as port:

	# 	while True:
	# 		sensor_data = port.readline()
	# 		aa = re.findall('[-+]?\d+\.\d+', sensor_data)
	# 		#print(aa)

			# t.header.stamp = rospy.Time.now()
			# t.header.frame_id = 'map'
			# t.child_frame_id = 'rov'
			# q = tf.transformations.quaternion_from_euler(float(aa[2])/180*3.1415926, float(aa[1])/180*3.1415926, -float(aa[0])/180*3.1415926)
			# print('x:', aa[2], 'y:', aa[1], 'z:', aa[0])
			# t.transform.rotation.x = q[0]
			# t.transform.rotation.y = q[1]
			# t.transform.rotation.z = q[2]
			# t.transform.rotation.w = q[3]
			# pose.orientation.x = float(aa[2])
			# pose.orientation.y = float(aa[1])
			# pose.orientation.z = -float(aa[0])

			# sensor_data = port.readline()
			# sensor_data = port.readline()
			# temperature = re.findall('[-+]?\d+\.\d+', sensor_data)			
			# sensor_data = port.readline()
			# t.transform.translation.x = 0
			# t.transform.translation.y = 0
			# depth = re.findall('[-+]?\d+\.\d+', sensor_data)[0]
			# t.transform.translation.z = -float(depth)
			# pose.position.z = -float(depth)
			# sensor_data = port.readline()
			# sensor_data = port.readline()
			# print('temperature: ', temperature[0], 'depth: ', depth)
			# br.sendTransform(t)
			# pub.publish(pose)
        
	# 		rospy.sleep(0.1)





