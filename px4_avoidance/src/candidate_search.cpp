#include "px4_avoidance/candidate_search.hpp"
#include <cmath>
#include <algorithm>

CandidateSearch::CandidateSearch(
        int sectors,
        double angle_min,
        double sector_angle)
{
    sectors_ = sectors;
    angle_min_ = angle_min;
    sector_angle_ = sector_angle;
}

std::vector<double> CandidateSearch::findCandidates(
        const std::vector<int>& B,
        int target_sector)
{
    std::vector<double> candidates;
    std::vector<std::pair<int,int>> openings;

    findOpenings(B, openings);

    int smax = std::round(16.0 / sector_angle_);

    for(const auto& op : openings)
    {
        int kr = op.first;
        int kl = op.second;

        int width = kl - kr + 1;

        if(width <= smax)
        {
            int kc = (kr + kl) / 2;

            double angle =
                angle_min_ + kc * sector_angle_;

            candidates.push_back(angle);
        }
        else
        {
            int cr = std::min(sectors_-1, kr + smax/2);
            int cl = std::max(0, kl - smax/2);

            double angle_r =
                angle_min_ + cr * sector_angle_;

            double angle_l =
                angle_min_ + cl * sector_angle_;

            candidates.push_back(angle_r);
            candidates.push_back(angle_l);

            if(target_sector >= cr && target_sector <= cl)
            {
                double angle_t =
                    angle_min_ +
                    target_sector * sector_angle_;

                candidates.push_back(angle_t);
            }
        }
    }

    return candidates;
}

void CandidateSearch::findOpenings(
        const std::vector<int>& hist,
        std::vector<std::pair<int,int>>& openings)
{
    int i = 0;

    while(i < sectors_)
    {
        while(i < sectors_ && hist[i] == 1)
            i++;

        if(i >= sectors_) break;

        int start = i;

        while(i < sectors_ && hist[i] == 0)
            i++;

        int end = i - 1;

        openings.push_back({start,end});
    }
}