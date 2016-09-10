//
// Created by Karol on 18.08.2016.
//

#ifndef SDLTEST_SDL_MEMORY_HPP
#define SDLTEST_SDL_MEMORY_HPP

#include <memory>
#include <SDL.h>

struct SDL_Window_deleter { void operator()(SDL_Window * lpWindow) { SDL_DestroyWindow(lpWindow); } };
using SDL_Window_uptr = ::std::unique_ptr<SDL_Window, SDL_Window_deleter>;

struct SDL_Renderer_deleter { void operator()(SDL_Renderer * lpRenderer) { SDL_DestroyRenderer(lpRenderer); } };
using SDL_Renderer_uptr = ::std::unique_ptr<SDL_Renderer, SDL_Renderer_deleter>;

struct SDL_Surface_deleter { void operator()(SDL_Surface * lpSurface) { SDL_FreeSurface(lpSurface); } };
using SDL_Surface_uptr = ::std::unique_ptr<SDL_Surface, SDL_Surface_deleter>;

struct SDL_Texture_deleter { void operator()(SDL_Texture * lpTexture) { SDL_DestroyTexture(lpTexture); } };
using SDL_Texture_uptr = ::std::unique_ptr<SDL_Texture, SDL_Texture_deleter>;

#endif // !SDLTEST_SDL_MEMORY_HPP