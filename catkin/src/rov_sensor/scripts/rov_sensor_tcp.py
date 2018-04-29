#!/usr/bin/env python  
import rospy
import math
import tf
import geometry_msgs.msg
import tf2_ros
import serial
import re
import struct
import socket
import os
from Tkinter import *

global w1 
global w2 
global w3 
global w4 
global w5
global d_hold
global pre_d
global ppre_d
global pre_h
global ppre_h
global depth
global heading

rospy.init_node('tfff')
address = ('192.168.0.100',12343)
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect(address)
br = tf2_ros.TransformBroadcaster()
t = geometry_msgs.msg.TransformStamped()
pub = rospy.Publisher('rov/sensors', geometry_msgs.msg.Pose, queue_size=10)
pose = geometry_msgs.msg.Pose()

def sel():
	global d_hold	
	w1.set(1460)
	w2.set(1460)
	w3.set(1460)
	w4.set(90)
	w5.set(90)
	d_hold = 0;
	pass
def dep():
	global d_hold
	if d_hold == 0:
		d_hold = 1
	else:
		d_hold = 0

def call(event):
#	res =  os.popen('vcgencmd measure_temp').readline()
#	print(res.replace("temp=","").replace("'c\n",""))
	global d_hold
	global pre_d
	global ppre_d
	global pre_h
	global ppre_h
	global depth
	global heading

	a = w1.get()
	b = w2.get()
	c = w3.get()
	d = w4.get()
	o1 = a + b - 1460
	o2 = 1460 - a + b
	if b>1460:
		o1 = o1-30
	elif b<1460:
		o2 = o2+30

	if d_hold != 0:
		if depth == -1:
			depth = pre_d
		if (depth - pre_d) > 0.02:
			c = 1530 + (depth - pre_d) * 300
		elif (depth - pre_d) < -0.06:
			c = 1405 + (depth - pre_d) * 50
		else:
			c = 1530
		if c > 1760:
			c = 1760
		if c < 1160:
			c = 1160
	if d_hold == 0:
		depth = -1
	
	if b == 1461:
		if heading == 500:			
			heading = pre_h
		err = heading - pre_h
		d_err = ppre_h - pre_h
		if err > 180:
			err = err - 360
		if err < -180:
                        err = err + 360
		if (err) > 1:
			o1 = 1510 + (err) * 1 + (1 + d_err * 9)
			o2 = o1
			#if o1 > 1460:
			#	o1 = o1-30
			#if o1 < 1460:
			#	o2 = o2+30
		elif (err) < -1:
			o1 = 1410 + (err) * 1 + (d_err * 9  - 1) 
			o2 = o1
			#if o1 > 1460:
			#	o1 = o1-30
			#if o1 < 1460:
			#	o2 = o2+30
		else:
			o1 = a + b - 1460
			o2 = 1460 - a + b
		if o1 > 1760:
			o1 = 1760
		if o1 < 1160:
			o1 = 1160
		if o2 > 1760:
			o2 = 1760
		if o2 < 1160:
			o2 = 1160
		print("----%06.3f   %d  %d   %06.3f    %06.3f"%(heading, o1, o2, err, d_err))

	else:
		heading = 500
		
	#print("****%06.3f   %d   %06.3f"%(depth, c, pre_d))
		
	temp = struct.pack("iiiiii", c, o2, o1, d, w5.get(),0)
	comd = b'\x0f'+temp+b'\n'
	#port.write(comd)
	s.sendall(comd)
	while True:
		#xx = port.read()
		xx = s.recv(1)
		if xx == b'\xf0':
			#xxx = port.read(20)
			xxx = s.recv(36)
			break
	# if len(xxx) == 20:
	# 	pass
	state = struct.unpack('fffffffff', xxx)
	ppre_d = pre_d	
	pre_d = state[4]
	ppre_h = pre_h	
	pre_h = state[2]
	print ('%06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f %06.3f'%state)

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

	global d_hold
	global pre_d
	global ppre_d
	global pre_h
	global ppre_h
	global depth
	global heading
	heading = 500
	depth = -1
	master = Tk()
	d = 0
	d_hold = 0
	pre_d = 0.0
	ppre_d = 0.0
	pre_h = 0.0
	ppre_h = 0.0

	w1 = Scale(master, from_=1260, to=1660, length=1000, label='motor1', orient=HORIZONTAL)
	w1.set(1460)
	w1.pack()

	w2 = Scale(master, from_=1260, to=1660, length=1000, label='motor2', orient=HORIZONTAL)
	w2.set(1460)
	w2.pack()
	w3 = Scale(master, from_=1260, to=1660, length=1000, label='motor3', orient=HORIZONTAL)
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
	#with serial.Serial('/dev/ttyUSB0', 115200) as port:
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





