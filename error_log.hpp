//
// Created by Karol on 18.08.2016.
//

#ifndef SDLTEST_ERROR_LOG_HPP
#define SDLTEST_ERROR_LOG_HPP

#include <cstdio>
#include <limits>

#include <ft2build.h>
#include FT_FREETYPE_H

#define putse(message) do { fputs(__FUNCTION__, stderr); fputs(": " message "\n", stderr); } while(false)
#define printfe(message, ...)  do { fputs(__FUNCTION__, stderr); fprintf(stderr, ": " message "\n", __VA_ARGS__); } while(false)

enum ST_EXIT_CODE : int {
    __STEC_ERR_LOWEST = ::std::numeric_limits<int>::min(),

    STEC_SDL_INIT, // Unable to initialize SDL
    STEC_SDL_IMAGE_INIT, // Unable to initialize SDL_Image
    STEC_SDL_WIN, // Window could not be created
    STEC_SDL_REN, // Renderer could not be created

    STEC_FF2_INIT, // Unable to initialize FreeType2
    STEC_TTF_DBG_LOAD,

    __STEC_ERR_HIGHEST,
    STEC_SUCCESS = 0
};
static_assert(__STEC_ERR_HIGHEST < 0, "Too many errors in enum ST_EXIT_CODE");
//inline bool isError(ST_EXIT_CODE stec) { return (int)stec < 0; }
//#define STEC_RETURN_ERROR(val) { const ST_EXIT_CODE tmp_stec = (val); if (isError(tmp_stec)) return tmp_stec; }

extern
const char * ft_error_to_string(FT_Error error) noexcept;

#endif // !SDLTEST_ERROR_LOG_HPP