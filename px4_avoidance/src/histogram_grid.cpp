#include "px4_avoidance/histogram_grid.hpp"
#include <cmath>
HistogramGrid::HistogramGrid()
{
    clear();
}
void HistogramGrid::clear()
{
    for(int i = 0; i < GRID; i++)
    {
        for(int j = 0; j < GRID; j++)
        {
            grid[i][j] = 0.0;
        }
    }
}
bool HistogramGrid::validIndex(int i, int j)
{
    return (i >= 0 && i < GRID && j >= 0 && j < GRID);
}
void HistogramGrid::build(const std::vector<Point2D>& obstacles)
{
    for(const auto& p : obstacles)
    {
        int i = center + static_cast<int>(std::round(p.x / resolution));
        int j = center + static_cast<int>(std::round(p.y / resolution));
        if(validIndex(i, j))
        {
            grid[i][j] += 1.0;
        }
    }
}