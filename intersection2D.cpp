//
// Created by Karol on 08.09.2016.
//

#include <cmath>
#include "intersection2D.hpp"

unsigned int line_segment_intersection(double l1x1, double l1y1, double l1x2, double l1y2,
                                       double l2x1, double l2y1, double l2x2, double l2y2, double & xi, double & yi) {
    // http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    double s1_x, s1_y, s2_x, s2_y;
    s1_x = l1x2 - l1x1;     s1_y = l1y2 - l1y1;
    s2_x = l2x2 - l2x1;     s2_y = l2y2 - l2y1;

    double s, t;
    s = (-s1_y * (l1x1 - l2x1) + s1_x * (l1y1 - l2y1)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (l1y1 - l2y1) - s2_y * (l1x1 - l2x1)) / (-s2_x * s1_y + s1_x * s2_y);

    if (0 <= s && s <= 1 && 0 <= t && t <= 1)
    {
        // Collision detected
        xi = l1x1 + (t * s1_x);
        yi = l1y1 + (t * s1_y);
        return 1u;
    }

    return 0u; // No collision
}

unsigned int line_segment_circle_intersection(double x1, double y1, double x2, double y2,
                                              double cx, double cy, double cr,
                                              double & xi1, double & yi1,
                                              double & xi2, double & yi2) {
    // http://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
    const double dx = x2 - x1;
    const double dy = y2 - y1;

    const double fx = x1 - cx;
    const double fy = y1 - cy;

    const double a = dx*dx + dy*dy;
    const double b = 2.0 * (fx*dx + fy*dy);
    const double c = fx*fx + fy*fy - cr*cr;

    const double det = b*b - 4.0*a*c;
    unsigned solutions = 0u;
    auto add_solution = [&solutions, &xi1, &yi1, &xi2, &yi2, x1, y1, dx, dy](double t){
        if (0.0 <= t && t <= 1.0) {
            switch (++solutions) {
                case 1:
                    xi1 = x1 + t*dx;
                    yi1 = y1 + t*dy;
                    break;
                case 2:
                    xi2 = x1 + t*dx;
                    yi2 = y1 + t*dy;
                    break;
                default:
                    __builtin_unreachable();
            }
        }
    };

    if (det == 0.0)
        add_solution((-b) / (2.0*a));
    else if (det > 0.0) {
        const double rdet = sqrt(det);
        add_solution((-b - rdet) / (2.0*a));
        add_solution((-b + rdet) / (2.0*a));
    }

    return solutions;
}