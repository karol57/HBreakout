//
// Created by Karol on 08.09.2016.
//

#define _USE_MATH_DEFINES
#include <cmath>

#include "SDL_helpers.hpp"

int SDL_RenderDrawCircle(SDL_Renderer * renderer, double x, double y, double r, unsigned sides) {
    if (sides == 0)
        sides = (unsigned)(2.0*M_PI*r / 2.0 + 0.5);

    double d_a = 2.0*M_PI/sides;
    double angle = d_a;

    double x1, y1;

    double x2 = r + x;
    double y2 = 0.0 + y;

    for (unsigned i = 0u; i < sides; ++i) {
        x1 = x2; y1 = y2;
        x2 = cos(angle) * r + x;
        y2 = sin(angle) * r + y;
        angle += d_a;
        const int result = SDL_RenderDrawLine(renderer, (int)(x1+0.5), (int)(y1+0.5), (int)(x2+0.5), (int)(y2+0.5));
        if (result != 0)
            return result;
    }

    return 0;
}