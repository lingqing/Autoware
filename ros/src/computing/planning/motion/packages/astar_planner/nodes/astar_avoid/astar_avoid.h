/*
*/

#ifndef ASTAR_AVOID_H
#define ASTAR_AVOID_H

#include <iostream>
#include <vector>
#include <thread>

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <autoware_msgs/LaneArray.h>

#include "waypoint_follower/libwaypoint_follower.h"
#include "astar_search.h"

class AstarAvoid
{
public:
  typedef enum STATE
  {
    INITIALIZING = -1,
    RELAYING = 0,
    STOPPING = 1,
    PLANNING = 2,
    AVOIDING = 3
  } State;

  AstarAvoid();
  ~AstarAvoid();
  void run();

private:
  // ros
  ros::NodeHandle nh_, private_nh_;
  ros::Publisher safety_waypoints_pub_;
  ros::Subscriber costmap_sub_;
  ros::Subscriber current_pose_sub_;
  ros::Subscriber current_velocity_sub_;
  ros::Subscriber base_waypoints_sub_;
  ros::Subscriber closest_waypoint_sub_;
  ros::Subscriber obstacle_waypoint_sub_;
  ros::Subscriber state_sub_;
  tf::TransformListener tf_listener_;

  // params
  int safety_waypoints_size_;
  double update_rate_;

  bool enable_avoidance_;
  int search_waypoints_size_;
  int search_waypoints_delta_;
  double avoid_waypoints_velocity_;
  double avoid_start_velocity_;
  double replan_interval_;

  // classes
  AstarSearch astar_;
  State state_;

  // variables
  bool found_avoid_path_;
  int closest_waypoint_index_;
  int obstacle_waypoint_index_;
  int goal_waypoint_index_;
  std::thread astar_thread_;
  nav_msgs::OccupancyGrid costmap_;
  autoware_msgs::Lane base_waypoints_;
  autoware_msgs::Lane safety_waypoints_;
  geometry_msgs::PoseStamped current_pose_local_, current_pose_global_;
  geometry_msgs::PoseStamped goal_pose_local_, goal_pose_global_;
  geometry_msgs::TwistStamped current_velocity_;
  tf::Transform local2costmap_;  // local frame (e.g. velodyne) -> costmap origin

  bool costmap_initialized_;
  bool current_pose_initialized_;
  bool current_velocity_initialized_;
  bool base_waypoints_initialized_;
  bool closest_waypoint_initialized_;

  // functions, callback
  void costmapCallback(const nav_msgs::OccupancyGrid& msg);
  void currentPoseCallback(const geometry_msgs::PoseStamped& msg);
  void currentVelocityCallback(const geometry_msgs::TwistStamped& msg);
  void baseWaypointsCallback(const autoware_msgs::Lane& msg);
  void closestWaypointCallback(const std_msgs::Int32& msg);
  void obstacleWaypointCallback(const std_msgs::Int32& msg);

  // functions
  bool checkInitialized();
  void startPlanThread(const autoware_msgs::Lane& current_waypoints, autoware_msgs::Lane& avoid_waypoints, int& end_of_avoid_index);
  void planWorker(const autoware_msgs::Lane& current_waypoints, autoware_msgs::Lane& avoid_waypoints, int& end_of_avoid_index, State& state);
  bool planAvoidWaypoints(const autoware_msgs::Lane& current_waypoints, autoware_msgs::Lane& avoid_waypoints, int& end_of_avoid_index);
  void mergeAvoidWaypoints(const nav_msgs::Path& path,const autoware_msgs::Lane& current_waypoints, autoware_msgs::Lane& avoid_waypoints, int& end_of_avoid_index);
  void publishWaypoints(const autoware_msgs::Lane& base_waypoints);
  tf::Transform getTransform(const std::string& from, const std::string& to);
};

#endif