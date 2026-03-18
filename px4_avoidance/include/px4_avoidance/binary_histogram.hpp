#pragma once

#include <vector>

class BinaryHistogram
{
public:
    BinaryHistogram(int sectors);
    void build(const std::vector<double>& polar_hist);
    std::vector<int> hist;
    double getThreshold() const { return T; }
private:
    double T;
    int sectors_;
};