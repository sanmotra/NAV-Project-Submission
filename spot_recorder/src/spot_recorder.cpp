#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <ros/ros.h>
#include <spot_recorder/SpotRecorder.h>

#include <fstream>

using namespace std;

ros::Subscriber pose_subscriber;

string filename = "/home/user/catkin_ws/spots.txt";
ofstream outfile;
bool recording = false;

geometry_msgs::PoseWithCovarianceStamped latest_pose_msg;

bool record_spot(spot_recorder::SpotRecorder::Request &req,
                 spot_recorder::SpotRecorder::Response &res) {
  if (req.label == "end") {
    outfile.close();
    res.navigation_successfull = true;
    res.message = "File saved successfully";
    recording = false;
  } else {
    // Set the response message
    res.navigation_successfull = true;
    res.message = "Spot saved successfully";

    if (!recording) {
      // Start recording and open the file for writing
      recording = true;
      outfile.open(filename.c_str(), ios::app);
    }
    // Write the current position and orientation to the file with label
    outfile << "Label Name : " << req.label << endl
            << "x : " << latest_pose_msg.pose.pose.position.x << endl
            << "y : " << latest_pose_msg.pose.pose.position.y << endl
            << "z : " << latest_pose_msg.pose.pose.orientation.z << endl
            << "w : " << latest_pose_msg.pose.pose.orientation.w << endl
            << endl;
  }
  return true;
}

void pose_callback(
    const geometry_msgs::PoseWithCovarianceStamped::ConstPtr &msg) {
  // Store the current pose message
  latest_pose_msg = *msg;
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "spot_recorder");
  ros::NodeHandle nh;

  // Open the output file
  outfile.open(filename.c_str());

  pose_subscriber = nh.subscribe("/amcl_pose", 1,
                                 pose_callback); // Subscribe to the pose topic

  ros::ServiceServer service = nh.advertiseService(
      "/save_spots", record_spot); // Create the service server

  recording = true; // Set recording flag to true

  ros::spin();

  return 0;
}
