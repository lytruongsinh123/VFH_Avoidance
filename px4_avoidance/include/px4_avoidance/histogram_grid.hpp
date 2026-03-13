#ifndef HISTOGRAM_GRID_HPP
#define HISTOGRAM_GRID_HPP
#include <vector>
#include "lidar_processing.hpp"
class HistogramGrid
{
public:
    static const int GRID = 601;
    HistogramGrid();
    void clear();
    void build(const std::vector<Point2D>& obstacles);
    double grid[GRID][GRID];
private:
    int center = (GRID - 1) / 2;
    double resolution = 0.1;
    bool validIndex(int i, int j);
};

#endif