//
// Created by Karol on 20.08.2016.
//

#ifndef SDLTEST_FONTGLYPH_HPP
#define SDLTEST_FONTGLYPH_HPP

#include <ft2build.h>
#include FT_TYPES_H
#include FT_GLYPH_H

#include <SDL_rect.h>

struct FontGlyph {
    enum STATUS {
        FGS_NON_EXISTENT,   // This glyph doesn't exist in font or couldn't be loaded
        FGS_EMPTY,          // This glyph exist in font but doesn't have any graphical representation (for example: space)
        FGS_NOT_PREPARED,   // This glyph exist in font, but isn't prepared to render
        FGS_PREPARED,       // This glyph exist in font and it's prepared to render
        FGS_RENDERED        // This glyph exist in font and it's rendered
    };

    FontGlyph() noexcept;
    FontGlyph(const FontGlyph&) = delete; // Currently not needed
    FontGlyph(FontGlyph&&) noexcept;
    ~FontGlyph() noexcept;

    FontGlyph& operator=(const FontGlyph&) = delete;
    FontGlyph& operator=(FontGlyph&&) noexcept;

    void init(FT_Face face, char32_t ch) noexcept;
    void prepare() noexcept;

    bool exist() const noexcept { return (m_status != FGS_NON_EXISTENT); }
    bool needPrepare() const noexcept { return (m_status == FGS_NOT_PREPARED); }
    bool hasGraphic() const noexcept {
        return (m_status == FGS_NOT_PREPARED) | (m_status == FGS_PREPARED) | (m_status == FGS_RENDERED);
    }
    bool ready() const noexcept  {
        return (m_status == FGS_PREPARED) | (m_status == FGS_RENDERED);
    }
    bool rendered() const noexcept { return m_status == FGS_RENDERED; }

    const SDL_Rect& texOff() const noexcept { return m_texoff; }

    FT_Int texDistLeft() const noexcept { return ((FT_BitmapGlyph)m_glyph)->left; }
    FT_Int texDistTop() const noexcept { return ((FT_BitmapGlyph)m_glyph)->top; }

    void setPos(int x, int y) noexcept {
        m_texoff.x = x;
        m_texoff.y = y;
    }

    const FT_Bitmap bitmap() const noexcept { return ((FT_BitmapGlyph)m_glyph)->bitmap; }

    void markNonExistent() noexcept { m_status = FGS_NON_EXISTENT; }
    void markRendered() noexcept { m_status = FGS_RENDERED; }

    const FT_Vector& advance() const noexcept { return m_glyph->advance; }

    private:
        STATUS      m_status;
        FT_UInt     m_cidx;   // Character index in font
        FT_Glyph    m_glyph;  // Glyph data
        FT_BBox     bbox;   // Bounding box of a glyph
        SDL_Rect    m_texoff;
};

#endif // !SDLTEST_FONTGLYPH_HPP