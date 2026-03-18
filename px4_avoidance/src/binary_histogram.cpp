#include "px4_avoidance/binary_histogram.hpp"
#include <algorithm>

BinaryHistogram::BinaryHistogram(int sectors)
{
    sectors_ = sectors;
    hist.resize(sectors_, 0);
    T = 0.0;
}
void BinaryHistogram::build(const std::vector<double>& polar_hist)
{
    double max_val = *std::max_element(polar_hist.begin(), polar_hist.end());
    T = 0.15 * max_val;
    for (int k = 0; k < sectors_; k++)
    {
        hist[k] = (polar_hist[k] >= T) ? 1 : 0;
    }
}