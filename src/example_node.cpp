

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "roboy_comm/Yaw.h"
#include "roboy_comm/Movement.h"
#include "roboy_comm/ShowEmotion.h"
#include "roboy_comm/Talk.h"

#include <sstream>

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
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
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

  ros::ServiceClient move_yaw_client = n.serviceClient<roboy_comm::Yaw>("roboy_move/yaw");
  ros::ServiceClient move_client = n.serviceClient<roboy_comm::Movement>("roboy_move/replay");
  ros::ServiceClient emotion_client = n.serviceClient<roboy_comm::ShowEmotion>("roboy_face/show_emotion");
  ros::ServiceClient talker_client = n.serviceClient<roboy_comm::Talk>("speech_synthesis/talk");

  ros::Rate loop_rate(1);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
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

    //ROS_INFO_STREAM("calling move_client");
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
