#ifndef POLAR_HISTOGRAM_HPP
#define POLAR_HISTOGRAM_HPP
#include "histogram_grid.hpp"
#include <vector>
class PolarHistogram
{
public:
    static constexpr double ANGLE_MIN = -135.0;
    static constexpr double ANGLE_MAX = 135.0;
    static constexpr double ALPHA = 15.0;      // sector width (deg)
    static constexpr int SECTOR = 18;         // 270° / 10°
    static constexpr double A = 2.8;
    static constexpr double B = 0.0005;
    static constexpr double RESOLUTION = 0.5;
    PolarHistogram();
    void clear();
    void build(double grid[HistogramGrid::GRID][HistogramGrid::GRID]);
    std::vector<double> hist;
};
#endif