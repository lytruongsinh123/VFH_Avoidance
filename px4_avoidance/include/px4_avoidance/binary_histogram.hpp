#pragma once

#include <vector>

class BinaryHistogram
{
public:
    BinaryHistogram(int sectors);
    void build(const std::vector<double>& polar_hist);
    std::vector<int> hist;
    double getTlow() const { return T_low; }
    double getThigh() const { return T_high; }
private:
    double T_low;
    double T_high;
    int sectors_;
};