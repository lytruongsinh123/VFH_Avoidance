#include "px4_avoidance/offboard_controll.hpp"

using namespace px4_msgs::msg;

OffboardControl::OffboardControl() : Node("offboard_control"), counter_(0)
{
    offboard_pub_ = this->create_publisher<OffboardControlMode>(
        "/fmu/in/offboard_control_mode", 10);

    traj_pub_ = this->create_publisher<TrajectorySetpoint>(
        "/fmu/in/trajectory_setpoint", 10);

    cmd_pub_ = this->create_publisher<VehicleCommand>(
        "/fmu/in/vehicle_command", 10);

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100),
        std::bind(&OffboardControl::timerCallback, this));
}

void OffboardControl::timerCallback()
{
    OffboardControlMode msg{};
    msg.position = false;
    msg.velocity = true;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    offboard_pub_->publish(msg);

    if(counter_ == 10)
    {
        setOffboardMode();
        arm();
    }

    counter_++;
}

void OffboardControl::sendVelocityCommand(double vx, double vy, double vz, double yaw)
{
    TrajectorySetpoint msg{};
    msg.velocity = { (float)vx, (float)vy, (float)vz };
    msg.yaw = yaw;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    traj_pub_->publish(msg);
}

void OffboardControl::arm()
{
    VehicleCommand msg{};
    msg.command = VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM;
    msg.param1 = 1.0;
    msg.target_system = 1;
    msg.target_component = 1;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    cmd_pub_->publish(msg);
}

void OffboardControl::setOffboardMode()
{
    VehicleCommand msg{};
    msg.command = VehicleCommand::VEHICLE_CMD_DO_SET_MODE;
    msg.param1 = 1;
    msg.param2 = 6; // OFFBOARD
    msg.target_system = 1;
    msg.target_component = 1;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    cmd_pub_->publish(msg);
}