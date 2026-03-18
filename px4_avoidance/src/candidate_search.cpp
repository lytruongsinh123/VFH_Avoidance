#include "px4_avoidance/candidate_search.hpp"
#include <cmath>
#include <algorithm>
#include "rclcpp/rclcpp.hpp"
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

    int smax = std::round(20.0 / sector_angle_);

    for(const auto& op : openings)
    {
        int kr = op.first;
        int kl = op.second;

        int width = kl - kr + 1;

        if(width <= smax)
        {
            double kc = (double)(kr + kl) / 2.0;

            double angle =
                angle_min_ + kc * sector_angle_;

            candidates.push_back(angle);
        }
        else
        {
            int cr = std::min(sectors_-1, kr + smax/2);
            int cl = std::max(0, kl - smax/2);
            
            // ===== IN RA =====
            RCLCPP_INFO(rclcpp::get_logger("candidate_search"),
            "Opening [%d, %d] | cr=%d cl=%d",
            kr, kl, cr, cl);
            double angle_r =
                angle_min_ + (cr) * sector_angle_;

            double angle_l =
                angle_min_ + (cl) * sector_angle_;

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