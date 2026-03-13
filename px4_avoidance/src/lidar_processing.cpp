#include "px4_avoidance/lidar_processing.hpp"
#include <cmath>
LidarProcessing::LidarProcessing(double max_range)
{
    this->max_range = max_range;
}
bool LidarProcessing::isValidRange(float r)
{
    if(!std::isfinite(r)) return false;
    if(r <= 0.0) return false;
    if(r > max_range) return false;
    return true;
}

std::vector<Point2D> LidarProcessing::processScan(
    const std::vector<float>& ranges,
    float angle_min,
    float angle_increment
)
{
    std::vector<Point2D> points;
    for(size_t i = 0; i < ranges.size(); i++)
    {
        float r = ranges[i];
        if(!isValidRange(r)) continue;
        float angle = angle_min + i*angle_increment;
        Point2D p;
        p.x = r * cos(angle);
        p.y = r * sin(angle);
        points.push_back(p);
    }
    return points;
}