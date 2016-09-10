//
// Created by Karol on 26.08.2016.
//

#ifndef SDLTEST_FONTMENAGER_HPP
#define SDLTEST_FONTMENAGER_HPP

#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>

#include "FontCache.hpp"

struct Font {
    unsigned fid;
    unsigned size = 12;
    SDL_Color color{ 255, 255, 255 };
};

#define HA_DEF    0b0000u
#define HA_LEFT   0b0010u
#define HA_RIGHT  0b0001u
#define HA_CENTER 0b0011u
#define HA_MASK   0b0011u

#define VA_DEF    0b0000u
#define VA_TOP    0b1000u
#define VA_BOTTOM 0b0100u
#define VA_MIDDLE 0b1100u
#define VA_MASK   0b1100u

class FontMenager {
public:
    FontMenager();
    ~FontMenager();

    SDL_Rect drawText(SDL_Renderer* renderer,
                      const Font& fnt, const ::std::u32string& text,
                      int x, int y, unsigned align = HA_DEF | VA_DEF);

    int getRAWTextWidth(unsigned fid, unsigned size, const char32_t * line_begin, const char32_t * line_end);

    unsigned loadFont(const char * path, FT_Long index = 0);
    void render(SDL_Renderer * renderer);

    FontCache& getCache(unsigned fid) noexcept { return m_faces.at(fid); }

    void drawRAWText(SDL_Renderer * renderer,
                     SDL_Texture * font_tex,
                     FontCache& fch, unsigned size,
                     const char32_t * line_begin, const char32_t * line_end,
                     int x, int y);

private:
    FT_Library m_ftlib;
    ::std::vector<FontCache> m_faces;
};

#endif // !SDLTEST_FONTMENAGER_HPP