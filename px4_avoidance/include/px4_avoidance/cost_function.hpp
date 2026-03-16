#pragma once
#include <vector>

class CostFunction
{
public:
    CostFunction(double mu1, double mu2, double mu3);

    double selectBest(
        const std::vector<double>& candidates,
        double target_angle,
        double robot_heading,
        double previous_angle);

private:
    double mu1_;
    double mu2_;
    double mu3_;
    double angleDiff(double a, double b);
};