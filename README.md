# Assembled 3d Lidar
This is a 3D indoor scanner that two servo motors are used to rotate and tilt the laser rangefinder. Arduino send each scan data to pc, and all the scans are megered in Point Cloud.    
![3D LIDAR](https://github.com/fan5ga/assembled_3D_lidar/blob/master/Lidar.jpg = 200px)


Folder catkin is  workspace of ROS, source code: catkin/src
- catkin/src/laser/laser_node.cpp: Laser node, Laser dat publisher    
- catkin/src/laser/laser_pd_merge_node.cpp laser data merge node    
- catkin/src/laser_to_pd/src/laser_to_pd_node.cpp: Point cloud registration.  
- catkin/src/sketch_jan04b/sketch_jan04b.ino: Arduino motor control   
**Scanning ranges: **
- Pitch:0-90degree(1 degree interval)
- Yaw: -90-90degree(1 degree interval )

### Dependency:
- ROS Kinetic
- [rosserial](http://wiki.ros.org/rosserial)


### Compile
- $ cd ~/catkin
- $ catkin_make

### Useage
- $ source ~/catkin/devel/setup.bash
- $ roscore
- $ rosrun tf static_transform_publisher /world /map 0 0 0 0 0 0 0 100
- $ rosrun laser laser_node
- $ rosrun laser_to_pd laser_to_pd_node 

### Check point cloud 
run RVIZ and select the laser node to visualize point cloud


