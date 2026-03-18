#pragma once

#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/offboard_control_mode.hpp"
#include "px4_msgs/msg/trajectory_setpoint.hpp"
#include "px4_msgs/msg/vehicle_command.hpp"

class OffboardControl : public rclcpp::Node
{
public:
    OffboardControl();

    void sendVelocityCommand(double vx, double vy, double vz, double yaw);

private:
    void timerCallback();

    void arm();
    void setOffboardMode();

private:
    rclcpp::Publisher<px4_msgs::msg::OffboardControlMode>::SharedPtr offboard_pub_;
    rclcpp::Publisher<px4_msgs::msg::TrajectorySetpoint>::SharedPtr traj_pub_;
    rclcpp::Publisher<px4_msgs::msg::VehicleCommand>::SharedPtr cmd_pub_;

    rclcpp::TimerBase::SharedPtr timer_;

    int counter_;
};