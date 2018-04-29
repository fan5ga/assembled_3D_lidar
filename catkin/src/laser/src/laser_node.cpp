#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include <tf/transform_broadcaster.h>
//#include "laser_geometry/laser_geometry.h"

#include "serial/serial.h"
#include <string>
#include <exception>  
#include <iostream>
const float TIME_INCREMENT = 0.1;
const float ANGLE_INCREMENT = 1.0/180.0*3.14;
class DIYLaser
{
private:
    ros::NodeHandle n_;
    ros::Publisher chatter_ ;
    sensor_msgs::LaserScan scan_pub_;
    std::string port_name_;
    serial::Serial *serial; 
    
    tf::TransformBroadcaster br_;
    tf::Transform transform_;

public:
    DIYLaser(std::string port_name = "/dev/ttyUSB0", const std::string baud = "115200")
    {
        std::cout<<"start"<<std::endl;
        chatter_ =  n_.advertise<sensor_msgs::LaserScan>("laser", 3);

        std::cout <<port_name<<std::endl;
        port_name_ = port_name;
        try
        {serial = new serial::Serial(port_name_.c_str(), atoi(baud.c_str()));}
        catch(std::exception& e){std::cout << "Standard exception: "<< std::endl; } 
        /*Time out = max*/
        serial->setTimeout(serial::Timeout::max(), serial::Timeout::max(), serial::Timeout::max(), serial::Timeout::max(), serial::Timeout::max());
        scan_pub_.range_min = 0.2;
        scan_pub_.range_max = 50;
        //scan_pub_.time_increment = TIME_INCREMENT;
        std::cout<< ANGLE_INCREMENT <<std::endl;
        std::cout<<int(3.14/ANGLE_INCREMENT)+1<<std::endl;
        scan_pub_.ranges.resize(int(3.14/ANGLE_INCREMENT)+1);
        scan_pub_.header.frame_id = "laser_frame";

        transform_.setOrigin( tf::Vector3(0.0, 0.0, 0.0) );

    }


    int publishScan()

    {
        while(true){
        //std::cout<<serial->available()<<std::endl;
            if(serial->available()>1 && n_.ok())
            {
                 //std::cout<<"start1"<<std::endl;
                std::string receive_buffer;
                int hasRead = serial->readline(receive_buffer);
                if (receive_buffer.at(0) != '*')continue;
                receive_buffer.erase(0,2);
                std::cout<<receive_buffer<<std::endl;
                char *buffer;
                //int number;
                char *readChars = (char*)receive_buffer.data();
                buffer = strtok(readChars, ",");
                int number = 0;

                scan_pub_.header.stamp = ros::Time::now();
                

                if (buffer == NULL) continue;

                while(buffer != NULL)
                {

                    if (0==number && (atoi(buffer)==1 || atoi(buffer)==-1))
                    {
                        int str2 = atoi(buffer);
                        //sscanf(buffer,"%d",&str2);
                        

                        scan_pub_.angle_increment = float(str2) * ANGLE_INCREMENT;
                        //std::cout<<scan_pub_.angle_increment<<std::endl;

                        1==str2 ? scan_pub_.angle_min=0   : scan_pub_.angle_min=3.14;
                        1==str2 ? scan_pub_.angle_max=3.14 : scan_pub_.angle_max=0;
                    } 
                    //else if(1==number  && is_number(buffer))
                    //{
                    //    scan_pub_.header.frame_id = atoi(buffer);
                   // }

                    else if (number>0 && number<int(3.14/ANGLE_INCREMENT+0.5)+2)
                    {
                        //std::cout<<3.14/ANGLE_INCREMENT<<std::endl;

                        scan_pub_.ranges[number-1] = atof(buffer);
                    }
                    
                    else if(number == int(3.14/ANGLE_INCREMENT+0.5)+2)
                    {
                        float str2 = atof(buffer);
                        tf::Quaternion q;
                        //(str2-60)/3.14
                        q.setRPY(float((str2-90)*3.14/180), 0, 0);
                        transform_.setRotation(q);
                        std::cout<<str2<<std::endl;
                    }                    
                    

                    
                  
                    number++;


                    buffer = strtok(NULL, ",");
                }
                

                scan_pub_.time_increment = (ros::Time::now() - scan_pub_.header.stamp).toSec()/(int(3.14/ANGLE_INCREMENT+0.5)+1);
                chatter_.publish(scan_pub_);
                br_.sendTransform(tf::StampedTransform(transform_, ros::Time::now(), "world", "laser_frame"));
                

            }
        }
        
        return 0;

    }

    bool is_number(const std::string& s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }

};

int main(int argc, char **argv)
{

    ros::init(argc, argv, "laser_node");
    if (argc >= 2) 
    {
        DIYLaser *laser_sample = new DIYLaser(std::string(argv[1]), std::string("115200"));
        laser_sample->publishScan();
    }

    else 
    {
        DIYLaser *laser_sample = new DIYLaser();
        laser_sample->publishScan();
    }
    


}
