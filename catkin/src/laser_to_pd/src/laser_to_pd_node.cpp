#include "ros/ros.h"
#include "tf/transform_listener.h"
#include <tf/transform_listener.h>
#include "sensor_msgs/PointCloud.h"
#include "sensor_msgs/LaserScan.h"
#include "tf/message_filter.h"
#include "message_filters/subscriber.h"
#include "laser_geometry/laser_geometry.h"

#include <iostream>

class LaserScanToPointCloud{

private:

  ros::NodeHandle n_;
  laser_geometry::LaserProjection projector_;

  ros::Subscriber laser_sub_;
  
  ros::Publisher scan_pub_;
  tf::TransformListener listener_;
  sensor_msgs::PointCloud merge_cloud_;
public:

  LaserScanToPointCloud() 
  {
    laser_sub_ = n_.subscribe<sensor_msgs::LaserScan>("/laser", 3, &LaserScanToPointCloud::scanCallback, this);
    scan_pub_ = n_.advertise<sensor_msgs::PointCloud>("/my_cloud",3, false);
    
   
    
  }

  void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_in)
  {

   if(!listener_.waitForTransform(
      scan_in->header.frame_id,
      "world",
      scan_in->header.stamp + ros::Duration().fromSec(scan_in->ranges.size()*scan_in->time_increment),
      ros::Duration(5))){
          std::cout<<"error"<<std::endl;
   return;
    }

    tf::StampedTransform transform;
    listener_.lookupTransform(scan_in->header.frame_id, "/world",scan_in->header.stamp + ros::Duration().fromSec(scan_in->ranges.size()*scan_in->time_increment), transform);
    tf::Quaternion q;
    q = transform.getRotation();
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    std::cout<<"roll"<<roll<<std::endl;
    m.getRPY(roll, pitch, yaw);





    //std::cout<<"callback";
    sensor_msgs::PointCloud cloud;
    sensor_msgs::PointCloud cloud2;
    try
    {
        projector_.projectLaser(*scan_in, cloud);
        cloud.header.frame_id = "world";
        cloudTransform(cloud, 0-roll);
        //listener_.transformPointCloud(scan_in->header.frame_id, cloud, cloud2);
        mergeCloud(cloud);
    }
    catch (tf::TransformException& e)
    {
        std::cout << e.what();
        return;
    }


    
    // // Do something with cloud.

     scan_pub_.publish(merge_cloud_);
     std::cout<<"points"<<merge_cloud_.points.size()<<std::endl;

  }

  void mergeCloud(const sensor_msgs::PointCloud &cloud)
  {

    if (merge_cloud_.points.size() == 0) merge_cloud_ = cloud;
    merge_cloud_.header = cloud.header;
    merge_cloud_.header.frame_id = "world";
    //std::cout<<cloud.header.frame_id<<endl;
    merge_cloud_.points.resize(merge_cloud_.points.size() + cloud.points.size());

    std::copy(cloud.points.begin(), cloud.points.end(), merge_cloud_.points.end() - cloud.points.size());

    for (unsigned int i = 0 ; i < cloud.channels.size() ; ++i)
    {
        merge_cloud_.channels[i].values.resize(cloud.channels[i].values.size() + merge_cloud_.channels[i].values.size());
        std::copy(cloud.channels[i].values.begin(), cloud.channels[i].values.end(), merge_cloud_.channels[i].values.end() - merge_cloud_.channels[i].values.size());
    }

    
  }

  void cloudTransform(sensor_msgs::PointCloud &cloud, const double roll)
  {
    for (int i=0; i<cloud.points.size(); i++)
    {
      double distance = pow(pow(cloud.points[i].x, 2) + pow(cloud.points[i].y, 2), 0.5);
      cloud.points[i].z = sin(roll) * distance;
      cloud.points[i].y = cos(roll) * cloud.points[i].y;
      cloud.points[i].x = cos(roll) * cloud.points[i].x;
    }
  }
};

int main(int argc, char** argv)
{
  std::cout<<"begin0"<<std::endl;
  ros::init(argc, argv, "laser_to_pd_node");
  std::cout<<"begin"<<std::endl;
  //ros::NodeHandle n;
  LaserScanToPointCloud lstopc;
  
  ros::spin();
  
  return 0;
}
