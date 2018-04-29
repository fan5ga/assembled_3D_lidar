#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "laser_geometry/laser_geometry.h"

#include "laser/SerialPort.h"
#include <string.h>

#define TIME_INCREMENT 0.1
#define ANGLE_INCREMENT 10
class DIYLaser:
{
private:
    ros::NodeHandle n_;
    ros::Publisher chatter_ = n.advertise<std_msgs::String>("laser", 10);
    sensor_msgs::LaserScan scan_sub_;
    SerialStream serial_port;
    string port_name_;
    SerialPort *serial; 

public:
    DIYLaser(const String port_name = "/dev/ttyUSB0", unsigned int baud = 9600)
    {
        port_name_ = port_name;
        serial = new Serial(port_name_.c_str(), baud);
        scan_pub_.range_min = 0.2;
        scan_pub_.range_max = 50;
        scan_pub_.time_increment = TIME_INCREMENT;
        scan_msg.ranges.resize(int(180/ANGLE_INCREMENT)+1);

    }


    int publishScan()

    {

        while(serial->isConnected() && serial->available()>1)
        {
            string receive_buffer;
            int hasRead = serial->readline(receive_buffer);
            char *buffer;
            int ;
            buffer = strtok(hasRead.c_str(), ",");
            int number = 0;

            scan_pub_.header.stamp = ros::Time();
            scan_pub_.scan_time = ros::Time();

            if (buffer == NULL) continue;

            while(buffer != NULL)
            {

                if (0==number && is_number(buffer))
                {
                    int str2 = std::stoi(buffer);

                    scan_pub_.angle_increment = str2 * ANGLE_INCREMENT;

                    1==str2 ? scan_pub_.angle_min=0   : scan_pub_.angle_min=180;
                    1==str2 ? scan_pub_.angle_max=180 : scan_pub_.angle_max=0;
                } 
                else if(1==num  && && is_number(buffer))
                {
                    scan_pub_.header.frame_id = atoi(buffer);
                }

                if (number>1 && number<int(180/ANGLE_INCREMENT)+3)
                {
                    scan_pub.ranges[number-2] = atof(buffer);
                }
                
                

                
              
                number++;


                buffer = strtok(hasRead.c_str(), ",");
            }

            scan_sub_.publish(scan_pub_);

        }
        
        return 0

    }
    bool is_number(const std::string& s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }

}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "DiyLaser");

    DiyLaser *laser_sample = new DIYLaser(argv[1], argv[2]);


}
