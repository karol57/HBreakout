//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_SDL_HELPERS_HPP
#define SDLTEST_SDL_HELPERS_HPP

#include <SDL_render.h>

/**
 *  \brief Draw a circle on the current rendering target.
 *
 *  \param renderer The renderer which should draw a line.
 *  \param x The x coordinate of the circle center.
 *  \param y The y coordinate of the circle center.
 *  \param r The radius of the circle.
 *  \param sides Count of segments of a circle
 *
 *  \return 0 on success, or -1 on error
 */
extern int SDL_RenderDrawCircle(SDL_Renderer * renderer, double x, double y, double r, unsigned sides = 0u);

#endif // !SDLTEST_SDL_HELPERS_HPP