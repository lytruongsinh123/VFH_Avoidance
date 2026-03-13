#pragma once

#include <vector>
class CandidateSearch
{
public:
    CandidateSearch(int sectors, double angle_min, double sector_angle);
    std::vector<double> findCandidates(const std::vector<int>& binary_hist);
private:
    int sectors_;
    double angle_min_;
    double sector_angle_;
};