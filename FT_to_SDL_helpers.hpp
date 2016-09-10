//
// Created by Karol on 18.08.2016.
//

#ifndef SDLTEST_FT_TO_SDL_HELPERS_HPP
#define SDLTEST_FT_TO_SDL_HELPERS_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL_render.h>
#include "SDL_memory.hpp"

bool ft_bitmap_rgba32_render(const FT_Bitmap& src, uint32_t* dst, int dst_pith);

#endif // !SDLTEST_FT_TO_SDL_HELPERS_HPP