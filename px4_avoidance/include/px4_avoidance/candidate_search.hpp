#pragma once
#include <vector>

class CandidateSearch
{
public:
    CandidateSearch(int sectors, double angle_min, double sector_angle);
    std::vector<double> findCandidates(
        const std::vector<int>& binary_hist,
        int target_sector);
private:
    int sectors_;
    double angle_min_;
    double sector_angle_;
    void findOpenings(
        const std::vector<int>& hist,
        std::vector<std::pair<int,int>>& openings);
};