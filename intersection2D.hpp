//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_INTERSECTION2D_HPP
#define SDLTEST_INTERSECTION2D_HPP

// Test for line segment & line segment collision
// (l1x1, l1y1), (l1x2;l1y2) - First line segment coordinates
// (l2x1, l2y1), (l2x2;l2y2) - Second line segment coordinates
// Returns number of intersections [0;1]
// Intersection coordinates are stored in (xi;yi)
extern unsigned int line_segment_intersection(double l1x1, double l1y1, double l1x2, double l1y2,
                                              double l2x1, double l2y1, double l2x2, double l2y2,
                                              double & xi, double & yi);

// Test for line segment & circle collision
// (x1,y1), (x2;y2) - line segment coordinates
// (cx, cy), cr - circle coordinates and radius
// Returns number of intersections [0;2]
// Intersections coordinates are stored in (xi1;yi1) and (xi2;yi2)
extern unsigned int line_segment_circle_intersection(double x1, double y1, double x2, double y2,
                                                     double cx, double cy, double cr,
                                                     double & xi1, double & yi1, double & xi2, double & yi2);

#endif // !SDLTEST_INTERSECTION2D_HPP