//
// Created by Karol on 20.08.2016.
//

#include <cstdint>
#include <cinttypes>

#include <new>
#include <utility>

#include "error_log.hpp"

#include "FontGlyph.hpp"

FontGlyph::FontGlyph() noexcept : m_status(FGS_NON_EXISTENT), m_cidx(0), m_glyph(nullptr) {}
FontGlyph::FontGlyph(FontGlyph&& m) noexcept :
        m_status(m.m_status), m_cidx(m.m_cidx),
        m_glyph(m.m_glyph), bbox(m.bbox), m_texoff(m.m_texoff) {
    new (&m) FontGlyph();
}
FontGlyph::~FontGlyph() noexcept {
    if (m_glyph)
        FT_Done_Glyph(m_glyph);
}

FontGlyph& FontGlyph::operator=(FontGlyph&& m) noexcept {
    this->~FontGlyph();
    new (this) FontGlyph(::std::move(m));

    return *this;
}

void FontGlyph::init(FT_Face face, char32_t ch) noexcept {
    this->~FontGlyph();

    m_cidx = FT_Get_Char_Index(face, ch);

    FT_Error error;
    if (m_cidx == 0) {
        printfe("Couldn't load character: 0x%" PRIX32, (std::uint32_t) ch);
        m_status = FontGlyph::FGS_NON_EXISTENT;
    }
    else if ((error = FT_Load_Glyph(face, m_cidx, FT_LOAD_DEFAULT)) != 0) {
        printfe("Couldn't load character 0x%" PRIX32 "(0x%X): %s",
                (std::uint32_t)ch, m_cidx, ft_error_to_string(error));
        m_status = FGS_NON_EXISTENT;
    }
    else if ((error = FT_Get_Glyph(face->glyph, &m_glyph)) != 0) {
        printfe("Couldn't get glyph 0x%" PRIX32 "(0x%X): %s",
                (std::uint32_t) ch, m_cidx, ft_error_to_string(error));
        m_status = FGS_NON_EXISTENT;
    }
    else {
        FT_Glyph_Get_CBox(m_glyph, FT_GLYPH_BBOX_PIXELS, &bbox);

        const long width  = bbox.xMax - bbox.xMin;
        const long height = bbox.yMax - bbox.yMin;

        if (width <= 0 || height <= 0)
            m_status = FontGlyph::FGS_EMPTY;
        else {
            m_texoff.w = width;
            m_texoff.h = height;

            m_status = (m_glyph->format == FT_GLYPH_FORMAT_BITMAP) ? FGS_PREPARED : FGS_NOT_PREPARED;
        }
    }
}

void FontGlyph::prepare() noexcept {
    FT_Error error = FT_Glyph_To_Bitmap(&m_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
    if (error != 0)
        printfe("Couldn't render glyph 0x%X: %s", m_cidx, ft_error_to_string(error));

    m_status = (error == 0) ? FGS_PREPARED : FGS_NON_EXISTENT;
}