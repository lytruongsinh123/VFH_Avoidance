#include "px4_avoidance/candidate_search.hpp"
CandidateSearch::CandidateSearch(int sectors, double angle_min, double sector_angle)
{
    sectors_ = sectors;
    angle_min_ = angle_min;
    sector_angle_ = sector_angle;
}
std::vector<double> CandidateSearch::findCandidates(const std::vector<int>& B)
{
    std::vector<double> candidates;
    int start = -1;
    for(int k = 0; k < sectors_; k++)
    {
        if(B[k] == 0 && start == -1)
        {
            start = k;
        }
        if((B[k] == 1 || k == sectors_-1) && start != -1)
        {
            int end = (B[k] == 1) ? k-1 : k;
            int width = end - start + 1;
            double angle_left  = angle_min_ + start * sector_angle_;
            double angle_right = angle_min_ + (end + 1) * sector_angle_;
            if(width <= 3)
            {
                double center = (angle_left + angle_right)/2.0;
                candidates.push_back(center);
            }
            else
            {
                double center = (angle_left + angle_right)/2.0;
                candidates.push_back(angle_left);
                candidates.push_back(center);
                candidates.push_back(angle_right);
            }
            start = -1;
        }
    }

    return candidates;
}