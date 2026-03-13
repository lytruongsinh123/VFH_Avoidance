#include "px4_avoidance/odom_tf.hpp"

OdomTF::OdomTF() : Node("odom_tf")
{
    tf_broadcaster_ =
        std::make_shared<tf2_ros::TransformBroadcaster>(this);
    rclcpp::QoS qos{rclcpp::SensorDataQoS()};
    sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
        "/fmu/out/vehicle_odometry",
        qos,
        std::bind(&OdomTF::callback, this, std::placeholders::_1));
}

void OdomTF::callback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg)
{
    geometry_msgs::msg::TransformStamped t;

    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = "world";
    t.child_frame_id = "base_link";

    t.transform.translation.x = msg->position[0];
    t.transform.translation.y = msg->position[1];
    t.transform.translation.z = -msg->position[2];

    t.transform.rotation.x = msg->q[1];
    t.transform.rotation.y = msg->q[2];
    t.transform.rotation.z = msg->q[3];
    t.transform.rotation.w = msg->q[0];

    tf_broadcaster_->sendTransform(t);
}
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<OdomTF>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}