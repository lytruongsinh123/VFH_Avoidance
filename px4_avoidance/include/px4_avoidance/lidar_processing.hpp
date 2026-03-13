#ifndef LIDAR_PROCESSING_HPP
#define LIDAR_PROCESSING_HPP

#include <vector>
struct Point2D
{
    double x;
    double y;
};
class LidarProcessing 
{
public:
    LidarProcessing(double max_range);
    std::vector<Point2D> processScan(
        const std::vector<float>& ranges,
        float angle_min,
        float angle_increment
    );
private:
    double max_range;
    bool isValidRange(float r);
};
#endif