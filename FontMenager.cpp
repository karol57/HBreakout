//
// Created by Karol on 26.08.2016.
//

#include <cstdlib>

#include "error_log.hpp"

#include "FontMenager.hpp"

static
unsigned u32StrCountLines(const std::u32string& str) {
    unsigned lines = 1;
    for (char32_t ch : str)
        if (ch == U'\n')
            ++lines;
    return lines;
}

static
std::u32string::const_iterator u32StrAdvancedCIterLineEnd(std::u32string::const_iterator it, const std::u32string::const_iterator end) {
    for (;it != end; ++it)
        if (*it == U'\n')
            break;
    return it;
}

FontMenager::FontMenager() {
    const FT_Error error = FT_Init_FreeType(&m_ftlib);
    if (error != 0) {
        fprintf(stderr, "Unable to initialize freetype: %i\n", error);
        exit(STEC_FF2_INIT);
    }
}
FontMenager::~FontMenager() {
    m_faces.clear();
    const FT_Error error = FT_Done_FreeType(m_ftlib);
    if (error != 0)
        fprintf(stderr, "Unable to free freetype: %i\n", error);
}

unsigned FontMenager::loadFont(const char * path, FT_Long index) {
    FT_Face face;
    FT_Error error = FT_New_Face(m_ftlib, path, index, &face);
    if (error != 0) {
        fprintf(stderr, "FontMenager: Couldn't load font %s:%li. FT_Error: %i\n", path, index, error);
        return ::std::numeric_limits<unsigned>::max();
    }

    const unsigned result = (unsigned)m_faces.size();
    m_faces.push_back(FontCache(face));
    return result;
}

void FontMenager::render(SDL_Renderer * renderer) {
    for (FontCache& face : m_faces)
        if (!face.isActual())
            face.rerenderTexture(renderer);
}

SDL_Rect FontMenager::drawText(SDL_Renderer* renderer,
                               const Font & fnt, const std::u32string& str,
                               const int orig_x, const int orig_y,
                               unsigned align) {
    if (fnt.fid >= m_faces.size())
        return {0, 0, 0, 0};

    FontCache& fch = m_faces[fnt.fid];
    const int lineHeight = fch.lineHeight(fnt.size) >> 6;

    SDL_Texture * const font_tex = fch.texture();

    SDL_SetTextureColorMod(font_tex, fnt.color.r, fnt.color.g, fnt.color.b);

    int y = [lineHeight, &str](unsigned valign, int originY){
        switch (valign) {
            case VA_DEF:
            case VA_TOP: return originY + lineHeight;
            default:;
        }

        const unsigned height = lineHeight * u32StrCountLines(str);
        switch (valign) {
            case VA_MIDDLE: return originY + lineHeight - (int)(height / 2u);
            case VA_BOTTOM: return originY + lineHeight - (int)height;
            default:;
        }
    }(align & VA_MASK, orig_y);

    for (auto it = str.cbegin(); it != str.cend(); ++it) {
        const char32_t * const line_begin = &*it;
        it = u32StrAdvancedCIterLineEnd(it, str.cend());
        const char32_t * const line_end = &*it;

        int x = [fnt, line_begin, line_end, this](unsigned halign, int originX) {
            switch (halign) {
                case HA_DEF:
                case HA_LEFT: return originX;
                default:;
            }

            const int width = getRAWTextWidth(fnt.fid, fnt.size, line_begin, line_end);
            switch (halign){
                case HA_CENTER: return originX - width/2;
                case HA_RIGHT: return originX - width;
                default:;
            }

            return originX;
        }(align & HA_MASK, orig_x);

        drawRAWText(renderer, font_tex, fch, fnt.size, line_begin, line_end, x, y);

        if (it == str.cend())
            break;

        y += lineHeight;
    }

    return {0,0,0,0};
}

int FontMenager::getRAWTextWidth(unsigned fid, unsigned size, const char32_t * line_begin, const char32_t * line_end) {
    if (fid >= m_faces.size())
        return ::std::numeric_limits<unsigned>::max();

    FontCache& fch = m_faces[fid];
    const bool kerning = (FT_HAS_KERNING(fch.face()) != 0);

    int width = 0;
    // int height = 0;

    char32_t ch_last = ::std::numeric_limits<char32_t>::max();
    for (const char32_t * it = line_begin; it != line_end; ++it) {
        const char32_t ch = *it;

        if (ch_last != ::std::numeric_limits<char32_t>::max() && kerning) {
            FT_Vector kVec;
            FT_Get_Kerning(fch.face(), ch_last, ch, FT_KERNING_DEFAULT, &kVec); // TODO: Handle error
            width += kVec.x >> 6;
            // height += kVec.y >> 6;
        }

        const FontGlyph & fg = fch.getGlyph(size, ch);
        if (fg.exist()) {
            width += fg.advance().x >> 16;
            // height += fg.advance().y >> 16;
        }

        ch_last = ch;
    }

    return width;
}
void FontMenager::drawRAWText(SDL_Renderer * renderer,
                              SDL_Texture * font_tex,
                              FontCache& fch, unsigned size,
                              const char32_t * line_begin, const char32_t * line_end,
                              int x, int y) {
    const bool kerning = (FT_HAS_KERNING(fch.face()) != 0);

    char32_t ch_last = ::std::numeric_limits<char32_t>::max();
    for (const char32_t * it = line_begin; it != line_end; ++it) {
        const char32_t ch = *it;

        if (kerning && ch_last != ::std::numeric_limits<char32_t>::max()) {
            FT_Vector kVec;
            FT_Get_Kerning(fch.face(), ch_last, ch, FT_KERNING_DEFAULT, &kVec); // TODO: Handle error
            x += kVec.x >> 6;
            y += kVec.y >> 6;
        }

        const FontGlyph & fg = fch.getGlyph(size, ch);
        if (fg.exist()) {
            if (fg.rendered()) {
                const SDL_Rect renderRC{x + fg.texDistLeft(), y - fg.texDistTop(),
                                        fg.texOff().w, fg.texOff().h};
                SDL_RenderCopy(renderer, font_tex, &fg.texOff(), &renderRC);
            }

            x += fg.advance().x >> 16;
            y += fg.advance().y >> 16;
        }

        ch_last = ch;
    }
}
