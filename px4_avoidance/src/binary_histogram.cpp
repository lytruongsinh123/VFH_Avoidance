#include "px4_avoidance/binary_histogram.hpp"
#include <algorithm>
BinaryHistogram::BinaryHistogram(int sectors)
{
    sectors_ = sectors;
    hist.resize(sectors_);
    T_low = 0.0;
    T_high = 0.0;
}
void BinaryHistogram::build(const std::vector<double>& polar_hist)
{
    double max_val = *std::max_element(polar_hist.begin(), polar_hist.end());
    // chọn threshold theo tỷ lệ
    T_low = 0.15 * max_val;
    T_high = 0.30 * max_val;
    for (int k = 0; k < sectors_; k++)
    {
        if (polar_hist[k] > T_high)
        {
            hist[k] = 1;
        }
        else if (polar_hist[k] < T_low)
        {
            hist[k] = 0;
        }
        else
        {
            if (k == 0)
                hist[k] = 0;
            else
                hist[k] = hist[k - 1];
        }
    }
}