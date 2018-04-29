#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <laser_geometry/laser_geometry.h>
#include <iostream>
using namespace std;

class My_Filter {
public:
    My_Filter(){
        scan_sub_ = node_.subscribe<sensor_msgs::LaserScan> ("/laser", 3, &My_Filter::scanCallback, this);
        point_cloud_publisher_ = node_.advertise<sensor_msgs::PointCloud> ("cloud", 3, false);
        //listener_.setExtrapolationLimit(ros::Duration(0.1));
    };
    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan){

        if(!listener_.waitForTransform(
        scan->header.frame_id,
        "world",
        scan->header.stamp + ros::Duration().fromSec(scan->ranges.size()*scan->time_increment),
        ros::Duration(5))){
            cout<<"error"<<endl;
     return;
        }
        sensor_msgs::PointCloud cloud;
        projector_.transformLaserScanToPointCloud("/world", *scan, cloud, listener_);
        mergeCloud(cloud);
        cout << "cloud"<<endl;
        point_cloud_publisher_.publish(merge_cloud_);
    };
    void mergeCloud(const sensor_msgs::PointCloud &cloud){

        if (merge_cloud_.points.size() == 0) merge_cloud_ = cloud;
        merge_cloud_.header = cloud.header;
        cout<<cloud.header.frame_id;
        merge_cloud_.points.resize(merge_cloud_.points.size() + cloud.points.size());

        std::copy(cloud.points.begin(), cloud.points.end(), merge_cloud_.points.end() - cloud.points.size());

        for (unsigned int i = 0 ; i < cloud.channels.size() ; ++i)
        {
            merge_cloud_.channels[i].values.resize(cloud.channels[i].values.size() + merge_cloud_.channels[i].values.size());
            std::copy(cloud.channels[i].values.begin(), cloud.channels[i].values.end(), merge_cloud_.channels[i].values.end() - merge_cloud_.channels[i].values.size());
        }

        
    }


private:
    ros::NodeHandle node_;
    laser_geometry::LaserProjection projector_;
    tf::TransformListener listener_;

    ros::Publisher point_cloud_publisher_;
    ros::Subscriber scan_sub_;
    sensor_msgs::PointCloud merge_cloud_;

};





int main(int argc, char** argv)
{
    ros::init(argc, argv, "my_filter");

    My_Filter filter;

    ros::spin();

    return 0;
}

