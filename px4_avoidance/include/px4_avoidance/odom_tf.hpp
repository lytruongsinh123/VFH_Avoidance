#ifndef ODOM_TF_HPP
#define ODOM_TF_HPP

#include <rclcpp/rclcpp.hpp>
#include <px4_msgs/msg/vehicle_odometry.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>

class OdomTF : public rclcpp::Node
{
public:
    OdomTF();
private:
    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr sub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    void callback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg);
};

#endif