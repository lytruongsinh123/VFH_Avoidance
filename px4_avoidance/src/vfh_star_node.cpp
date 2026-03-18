#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nav_msgs/msg/grid_cells.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

#include "px4_avoidance/lidar_processing.hpp"
#include "px4_avoidance/histogram_grid.hpp"
#include "px4_avoidance/polar_histogram.hpp"
#include "px4_avoidance/binary_histogram.hpp"
#include "px4_avoidance/candidate_search.hpp"
#include "px4_avoidance/cost_function.hpp"
#include "px4_avoidance/offboard_controll.hpp"

#include <memory>
#include <cmath>
#include <algorithm>
class VFHStarNode : public rclcpp::Node
{

public:

    VFHStarNode(std::shared_ptr<OffboardControl> offboard) 
    : Node("vfh_star_node"),
    offboard_(offboard), 
    lidar_(30.0), 
    binary_hist_(27), 
    candidate_search_(27, -135.0, 10.0), 
    cost_function_(5.0, 2.0, 2.0),
    current_altitude_(0.0),
    current_yaw_(0.0)
    {

        scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/world/default/model/x500_lidar_2d_0/link/link/sensor/lidar_2d_v2/scan",
            10,
            std::bind(&VFHStarNode::scanCallback, this, std::placeholders::_1)
        );
        odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom",
            10,
            std::bind(&VFHStarNode::odomCallback, this, std::placeholders::_1)
        );
        marker_pub_ = this->create_publisher<visualization_msgs::msg::Marker>(
            "/lidar_points",
            10
        );
        hist_pub_ = this->create_publisher<visualization_msgs::msg::Marker>(
            "/polar_histogram",
            10
        );
        grid_obstacle_pub_ = this->create_publisher<nav_msgs::msg::GridCells>(
            "/grid_obstacle",
            10
        );
        grid_empty_pub_ = this->create_publisher<nav_msgs::msg::GridCells>(
            "/grid_empty",
            10
        );
        plot_pub_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/polot_histogram", 
            10
        );
        candidate_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
            "/candidate_direction",
            10
        );
        best_dir_pub_ = this->create_publisher<visualization_msgs::msg::Marker>(
            "/best_direction",
            10
        );
    }

private:

    void scanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {

        auto points = lidar_.processScan(
            msg->ranges,
            msg->angle_min,
            msg->angle_increment
        );

        // RCLCPP_INFO(this->get_logger(), "Points received: %ld", points.size());

        // Build Histogram Grid
        histogram_.clear();
        histogram_.build(points);

        // Build Polar Histogram
        polar_hist_.clear();
        polar_hist_.build(histogram_.grid);
        
        // Plot
        std_msgs::msg::Float32MultiArray plot_msg;
        plot_msg.data.assign(
            polar_hist_.hist.begin(),
            polar_hist_.hist.end()
        );
        // thêm threshold
        plot_msg.data.push_back(binary_hist_.getThreshold());
        plot_pub_->publish(plot_msg);

        // Build Binary Histogram
        binary_hist_.build(polar_hist_.hist);
        

        // Target direction
        double target_angle = 0.0;
        int target_sector =
            (target_angle - PolarHistogram::ANGLE_MIN) / PolarHistogram::ALPHA;

        // Candidate search
        auto candidates =
            candidate_search_.findCandidates(
                binary_hist_.hist,
                target_sector
        );

        // Robot heading (tạm thời)
        double robot_heading = current_yaw_ * 180.0 / M_PI;

        // Cost function
        double best_direction =
            cost_function_.selectBest(
                candidates,
                target_angle,
                robot_heading,
                previous_direction_
        );
        // cập nhật hướng trước
        previous_direction_ = best_direction;
        
        
        // UAV CONTROL
        double best_direction_rad = best_direction * M_PI / 180.0;
        double yaw = current_yaw_ + best_direction_rad;
        double min_dist = 100.0;
        for(auto r : msg->ranges)
        {
            if(std::isfinite(r))
                min_dist = std::min(min_dist, (double)r);
        }

        double speed = 2.0;
        if(min_dist < 2.0) speed = 1.0;
        if(min_dist < 1.0) speed = 0.5;

        // ===== OFFBOARD COMMAND =====
        double vx = cos(yaw) * speed;
        double vy = sin(yaw) * speed;
        double vz = -0.5;   // 🔥 bay lên (NED)

        offboard_->sendVelocityCommand(
            vx, vy, vz, yaw
        );


        // Publish
        publishPoints(points);
        publishGrid();
        publishPolarHistogram();
        publishCandidates(candidates);
        publishBestDirection(best_direction);
    }

    void publishPoints(const std::vector<Point2D>& points)
    {

        visualization_msgs::msg::Marker marker;

        marker.header.frame_id = "base_link";
        marker.header.stamp = this->now();

        marker.ns = "lidar_points";
        marker.id = 0;

        marker.type = visualization_msgs::msg::Marker::POINTS;
        marker.action = visualization_msgs::msg::Marker::ADD;

        marker.scale.x = 0.05;
        marker.scale.y = 0.05;

        marker.color.r = 1.0;
        marker.color.g = 0.0;
        marker.color.b = 0.0;
        marker.color.a = 1.0;

        for (const auto &p : points)
        {
            geometry_msgs::msg::Point pt;

            pt.x = p.x;
            pt.y = p.y;
            pt.z = current_altitude_;

            marker.points.push_back(pt);
        }

        marker_pub_->publish(marker);
    }

    void publishGrid()
    {

        nav_msgs::msg::GridCells obstacle_cells;
        nav_msgs::msg::GridCells empty_cells;

        int center = HistogramGrid::GRID / 2;
        double res = 0.1;

        obstacle_cells.header.frame_id = "base_link";
        obstacle_cells.header.stamp = this->now();
        obstacle_cells.cell_width = res;
        obstacle_cells.cell_height = res;

        empty_cells = obstacle_cells;

        for(int i = 0; i < HistogramGrid::GRID; i++)
        {
            for(int j = 0; j < HistogramGrid::GRID; j++)
            {

                geometry_msgs::msg::Point pt;

                pt.x = (i - center) * res;
                pt.y = (j - center) * res;
                pt.z = current_altitude_;

                if(histogram_.grid[i][j] > 0)
                    obstacle_cells.cells.push_back(pt);
                else
                    empty_cells.cells.push_back(pt);
            }
        }

        grid_obstacle_pub_->publish(obstacle_cells);
        grid_empty_pub_->publish(empty_cells);
    }

    void publishPolarHistogram()
    {

        visualization_msgs::msg::Marker marker;

        marker.header.frame_id = "base_link";
        marker.header.stamp = this->now();

        marker.ns = "polar_histogram";
        marker.id = 0;

        marker.type = visualization_msgs::msg::Marker::LINE_LIST;
        marker.action = visualization_msgs::msg::Marker::ADD;

        marker.scale.x = 0.05;

        marker.color.r = 1.0;
        marker.color.g = 0.0;
        marker.color.b = 0.0;
        marker.color.a = 1.0;

        geometry_msgs::msg::Point origin;

        origin.x = 0;
        origin.y = 0;
        origin.z = current_altitude_;

        for(int k = 0; k <= PolarHistogram::SECTOR; k++)
        {

            double angle =
                (PolarHistogram::ANGLE_MIN + k * PolarHistogram::ALPHA)
                * M_PI / 180.0;

            double length = polar_hist_.hist[k] * 0.1;

            geometry_msgs::msg::Point end;

            end.x = cos(angle) * 6;
            end.y = sin(angle) * 6;
            end.z = current_altitude_;

            marker.points.push_back(origin);
            marker.points.push_back(end);
        }

        hist_pub_->publish(marker);
    }

    void publishCandidates(const std::vector<double>& candidates)
    {
        visualization_msgs::msg::MarkerArray marker_array;
        int id = 0;
        for(double angle_deg : candidates)
        {
            visualization_msgs::msg::Marker marker;
            marker.header.frame_id = "base_link";
            marker.header.stamp = this->now();
            marker.ns = "candidate";
            marker.id = id++;
            marker.type = visualization_msgs::msg::Marker::ARROW;
            marker.action = visualization_msgs::msg::Marker::ADD;
            double angle = angle_deg * M_PI / 180.0;
            geometry_msgs::msg::Point p1, p2;
            p1.x = 0;
            p1.y = 0;
            p1.z = current_altitude_;
            p2.x = 4 * cos(angle);
            p2.y = 4 * sin(angle);
            p2.z = current_altitude_;
            marker.points.push_back(p1);
            marker.points.push_back(p2);
            marker.scale.x = 0.1;
            marker.scale.y = 0.2;
            marker.scale.z = 0.2;

            marker.color.a = 1.0;
            marker.color.r = 0.0;
            marker.color.g = 0.0;
            marker.color.b = 1.0;
            marker_array.markers.push_back(marker);
        }
        candidate_pub_->publish(marker_array);
    }

    void publishBestDirection(double angle_deg)
    {
        visualization_msgs::msg::Marker marker;

        marker.header.frame_id = "base_link";
        marker.header.stamp = this->now();

        marker.ns = "best_direction";
        marker.id = 0;

        marker.type = visualization_msgs::msg::Marker::ARROW;
        marker.action = visualization_msgs::msg::Marker::ADD;

        double angle = angle_deg * M_PI / 180.0;

        geometry_msgs::msg::Point p1, p2;

        p1.x = 0;
        p1.y = 0;
        p1.z = current_altitude_;

        p2.x = 5 * cos(angle);
        p2.y = 5 * sin(angle);
        p2.z = current_altitude_;

        marker.points.push_back(p1);
        marker.points.push_back(p2);

        marker.scale.x = 0.15;
        marker.scale.y = 0.3;
        marker.scale.z = 0.3;

        marker.color.a = 1.0;
        marker.color.r = 0.0;
        marker.color.g = 1.0;
        marker.color.b = 0.0;

        best_dir_pub_->publish(marker);
    }

    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
         current_altitude_ = msg->pose.pose.position.z;

        const auto &q = msg->pose.pose.orientation;
        tf2::Quaternion quat(q.x, q.y, q.z, q.w);

        double roll, pitch, yaw;
        tf2::Matrix3x3(quat).getRPY(roll, pitch, yaw);

        current_yaw_ = yaw;
    }


    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_pub_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr hist_pub_;
    rclcpp::Publisher<nav_msgs::msg::GridCells>::SharedPtr grid_obstacle_pub_;
    rclcpp::Publisher<nav_msgs::msg::GridCells>::SharedPtr grid_empty_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr plot_pub_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr candidate_pub_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr best_dir_pub_;
    
    std::shared_ptr<OffboardControl> offboard_;
    LidarProcessing lidar_;
    HistogramGrid histogram_;
    PolarHistogram polar_hist_;
    BinaryHistogram binary_hist_;
    CandidateSearch candidate_search_;
    CostFunction cost_function_;


    double current_altitude_;
    double current_yaw_;
    double previous_direction_ = 0.0;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto offboard_node = std::make_shared<OffboardControl>();
    auto node = std::make_shared<VFHStarNode>(offboard_node);
    
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.add_node(offboard_node);
    executor.spin();

    rclcpp::shutdown();
    return 0;
}