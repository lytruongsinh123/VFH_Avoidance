#include "px4_avoidance/polar_histogram.hpp" 
#include <cmath> 
#include <algorithm> 
PolarHistogram::PolarHistogram() 
{ 
    hist.resize(SECTOR, 0.0); 
} 
void PolarHistogram::clear() 
{ 
    std::fill(hist.begin(), hist.end(), 0.0); 
} 
void PolarHistogram::build(double grid[HistogramGrid::GRID][HistogramGrid::GRID]) 
{ 
    clear(); 
    int GRID = HistogramGrid::GRID; 
    int center = GRID / 2; 
    for(int i = 0; i < GRID; i++) 
    { 
        for(int j = 0; j < GRID; j++) 
        { 
            double c = grid[i][j]; 
            if(c <= 0) continue; 
            double dx = (i - center) * RESOLUTION + RESOLUTION / 2.0; 
            double dy = (j - center) * RESOLUTION + RESOLUTION / 2.0; 
            double d = sqrt(dx*dx + dy*dy); 
            
            // VFH obstacle density 
            double m = c * c * (A - B * d); 
            if(m <= 0) continue; 
            double angle = atan2(dy, dx) * 180.0 / M_PI; 
            if(angle < ANGLE_MIN || angle > ANGLE_MAX) continue; 
            int k = floor((angle - ANGLE_MIN) / ALPHA);
            if(k >= SECTOR) k = SECTOR - 1; 
            hist[k] += m; 
        } 
    } 
}