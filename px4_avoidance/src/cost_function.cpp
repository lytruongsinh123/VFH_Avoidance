#include "px4_avoidance/cost_function.hpp"
#include <cmath>
#include <limits>

CostFunction::CostFunction(double mu1, double mu2, double mu3)
{
    mu1_ = mu1;
    mu2_ = mu2;
    mu3_ = mu3;
}

double CostFunction::angleDiff(double a, double b)
{
    double diff = fabs(a - b);

    while(diff > 180.0)
        diff -= 360.0;

    return fabs(diff);
}

double CostFunction::selectBest(
        const std::vector<double>& candidates,
        double target_angle,
        double robot_heading,
        double previous_angle)
{
    double best_cost = std::numeric_limits<double>::max();
    double best_direction = 0.0;

    for(double c : candidates)
    {
        double cost =
              mu1_ * angleDiff(c, target_angle)
            + mu2_ * angleDiff(c, robot_heading)
            + mu3_ * angleDiff(c, previous_angle);

        if(cost < best_cost)
        {
            best_cost = cost;
            best_direction = c;
        }
    }

    return best_direction;
}