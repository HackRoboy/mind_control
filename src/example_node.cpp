int main(){
    return 0;
}

#if 0

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "roboy_comm/Yaw.h"
#include "roboy_comm/Movement.h"
#include "roboy_comm/ShowEmotion.h"
#include "roboy_comm/Talk.h"

#include <sstream>



/*
 * Comm.cpp
 *
 *  Created on: 29-04-2017
 *      Author: mayank
 */
#include <SerialStream.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>

#include <sstream>

 using namespace std;
 using namespace LibSerial ;
int curr_state_head_yaw = 0;

ros::ServiceClient move_yaw_client;
double THRESHOLD_1 = 220;
std::vector<int> last_bytes;
int counter_above_th = 0;
int counter_all = 0;

void move_head(int pos){
    if(ros::ok()){
        ROS_INFO_STREAM("Moving head: " << pos);
        roboy_comm::Yaw srv;
        srv.request.value = pos;
        move_yaw_client.call(srv);
    }

}

void move_head_left(bool left){
    bool real_left = false;
    //ROS_INFO_STREAM("counter: " << counter_all << " counter_abv: " << counter_above_th);
    if(counter_all < 100){
        if(left){
            counter_above_th++;
        }
        counter_all++;
        return;
    }else{
        if(counter_above_th>10){
            real_left = true;
        }
        ROS_INFO_STREAM("Counter_Above_th: " << counter_above_th);
        counter_all = 0;
        counter_above_th = 0;
    }
    if(real_left){
        curr_state_head_yaw +=1;
        if(curr_state_head_yaw>12){
            curr_state_head_yaw = 12;
        }
    }else{
        curr_state_head_yaw -=1;
        if(curr_state_head_yaw<-12){
            curr_state_head_yaw = -12;
        }
    }
    move_head(curr_state_head_yaw);
}
#endif
#if 0
int main( int    argc, char** argv  ){

    ros::init(argc, argv, "mind_control");

    /**
     * NodeHandle is the main access point to communications with the ROS system.
     * The first NodeHandle constructed will fully initialize this node, and the last
     * NodeHandle destructed will close down the node.
     */
    ros::NodeHandle n;
    ROS_INFO("INIT");
    std::cout << "init" << std::endl;

     //
     // Open the serial port.
     //
     SerialStream serial_port ;
     char c;
     serial_port.Open( "/dev/ttyACM2" ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
                   << "Error: Could not open serial port."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Set the baud rate of the serial port.
     //
     serial_port.SetBaudRate( SerialStreamBuf::BAUD_9600 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the baud rate." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of data bits.
     //
     serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the character size." <<
std::endl ;
         exit(1) ;
     }
     //
     // Disable parity.
     //
     serial_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not disable the parity." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of stop bits.
     //
     serial_port.SetNumOfStopBits( 1 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the number of stop bits."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Turn off hardware flow control.
     //
     serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not use hardware flow control."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Do not skip whitespace characters while reading from the
     // serial port.
     //
     // serial_port.unsetf( std::ios_base::skipws ) ;
     //
     // Wait for some data to be available at the serial port.
     //
     //
     // Keep reading data from serial port and print it to the screen.
     //
  // Wait for some data to be available at the serial port.
     //

     // ServiceClient to give roboy commands
     move_yaw_client = n.serviceClient<roboy_comm::Yaw>("roboy_move/yaw");


     while( serial_port.rdbuf()->in_avail() == 0 )
     {
         usleep(100) ;
     }

         ofstream writer;
         writer.open("Log.txt");
     char out_buf[] = "check";
     serial_port.write(out_buf, 5);
     std::stringstream ss;
     //while( serial_port.rdbuf()->in_avail() > 0 )
     while(1)
     {
         //ROS_INFO("while loop");
         char next_byte;
         serial_port.get(next_byte);
         if(next_byte=='\n'){
             //std::cout << std::endl;
             //calculate mean and decide action
             double mean = 0;
             //for(int i = 0; i < last_bytes.size(); i++){
             //    std::cout << last_bytes[i] << " ";
             //}
             //std::cout << std::endl;

             for(int i = 0; i < 6; i++){
                 try{
                    mean += last_bytes[1+i*2];
                 }catch(exception e){
                     std:cerr << "exception" << std::endl;
                 }

                 //std::cout<<(int)last_bytes[1+i*2] << ", ";
             }
             //std::cout << std::endl;
             mean /=6;

             //ROS_INFO_STREAM("Mean: " << mean);

             if(mean>THRESHOLD_1){
                 //ROS_INFO("MOVE HEAD LEFT");
                 move_head_left(true);
                 //move_head(8);
                 //sleep(1);
             }else{
                 //ROS_INFO("MOVE HEAD RIGHT");
                 move_head_left(false);
                 //sleep(1);
             }
             //clear last_bytes
             last_bytes.clear();
         }else{
             //collect data
             if(next_byte != ','){
                 ss << next_byte;
            }else{
                 //std::cout << ss.str().c_str() << ",";
                 try{
                 last_bytes.push_back(atoi(ss.str().c_str()));
                 }catch(exception e){
                     std::cerr << "couldnt convert" << std::endl;
                 }

                 ss.str(std::string());
             }
         }
         //std::cerr << next_byte;
                    writer<<next_byte;




     }
     //std::cerr << std::endl ;
             writer.close();
     return EXIT_SUCCESS ;
}


#endif

#if 0

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
  std::cout << "starting" << std::endl;
  ROS_INFO("Starting");
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");


  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  ROS_INFO("Handle created");
  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ROS_INFO("init");
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

  move_yaw_client = n.serviceClient<roboy_comm::Yaw>("roboy_move/yaw");
  ros::ServiceClient move_client = n.serviceClient<roboy_comm::Movement>("roboy_move/replay");
  ros::ServiceClient emotion_client = n.serviceClient<roboy_comm::ShowEmotion>("roboy_face/show_emotion");
  ros::ServiceClient talker_client = n.serviceClient<roboy_comm::Talk>("speech_synthesis/talk");

  ros::Rate loop_rate(1);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 100;
  bool left = false;
  while (ros::ok())
  {

      if(count==100){
          ROS_INFO_STREAM("calling move introduction");
          roboy_comm::Movement move_srv;
          move_srv.request.value = "TheatreWave";
          if(move_client.call(move_srv)){
              ROS_INFO("Call valid!");
          }
          if(move_srv.response.success){
              ROS_INFO("success");
          }
          return 0;
      }

    roboy_comm::Yaw move_yaw_srv;
    int count_2 = 8;
    if(left){
        count_2=-8;
    }
    left = !left;

    move_yaw_srv.request.value = count_2;

    ROS_INFO_STREAM("calling move_client");
    move_yaw_client.call(move_yaw_srv);

    roboy_comm::ShowEmotion emotion_srv;
    roboy_comm::Talk talk_srv;

    if(false){
        ROS_INFO("SmileBlink");
        emotion_srv.request.emotion = "smileblink";
        emotion_client.call(emotion_srv);

        talk_srv.request.text = "Hallo";
        talker_client.call(talk_srv);
    }

    count++;
    ros::spinOnce();

    loop_rate.sleep();
  }


  return 0;
}
#endif
