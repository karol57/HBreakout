//
// Created by Karol on 18.08.2016.
//

#include <cstdint>
#include <cinttypes>

#include <vector>
#include <utility>

#include "error_log.hpp"
#include "FT_to_SDL_helpers.hpp"

#include "FontCache.hpp"

static
unsigned organizeGlyphs(unsigned texSize, ::std::vector<FontGlyph*>& glyphs) {
    // Calculating size of texture

    tex_size_calculation:
    {
        unsigned x = 0;
        unsigned y = 0;
        unsigned max_h = 0;

        for (FontGlyph* glyph : glyphs) {


            if (x + glyph->texOff().w > texSize) {
                x = 0;
                y += max_h;
                max_h = 0;
            }

            if (y + glyph->texOff().h > texSize) {
                texSize *= 2;
                goto tex_size_calculation;
            }

            glyph->setPos(x, y);
            x += glyph->texOff().w;
            max_h = ::std::max(max_h, (unsigned)glyph->texOff().h);
        }
    }

    return texSize;
}

SizedFontMap::SizedFontMap(int lineHeight) : m_lineHeight(lineHeight) {}
SizedFontMap::SizedFontMap(int lineHeight, char32_t fch, FontGlyph && ffg) : SizedFontMap(lineHeight) {
    m_map.emplace(fch, ::std::move(ffg));
}
SizedFontMap::SizedFontMap(SizedFontMap&&) = default;
SizedFontMap::~SizedFontMap() = default;

SizedFontMap& SizedFontMap::operator=(SizedFontMap&&) = default;

FontCache::FontCache(FT_Face face) : m_face(face), m_isActual(true), m_texSize(0) {}
FontCache::FontCache(FontCache && m) : m_face(m.m_face),
                                       m_glyphs(::std::move(m.m_glyphs)), m_texture(::std::move(m.m_texture)),
                                       m_isActual(m.m_isActual), m_texSize(m.m_texSize) {
    m.m_face = nullptr;
    m.m_isActual = true;
    m.m_texSize = 0;
}

FontCache& FontCache::operator=(FontCache && m) {
    this->~FontCache();

    m_face = m.m_face;
    m_glyphs = ::std::move(m.m_glyphs);
    m_texture = ::std::move(m.m_texture);
    m_isActual = m.m_isActual;
    m_texSize = m.m_texSize;

    m.m_face = nullptr;
    m.m_isActual = true;
    m.m_texSize = 0;

    return *this;
}

FontCache::~FontCache() {
    if (m_face)
        FT_Done_Face(m_face);
}

int FontCache::lineHeight(unsigned size) {
    const auto fit = m_glyphs.find(size);
    if (fit != m_glyphs.cend())
        return fit->second.lineHeight();

    FT_Error error;
    if ((error = FT_Set_Char_Size(m_face, (FT_F26Dot6)(size << 6), (FT_F26Dot6)(size << 6), 96, 96)) != 0)
        printfe("Setting character size (%u pt) failed: %s.", size, ft_error_to_string(error));
    return m_glyphs.emplace(size, SizedFontMap{m_face->size->metrics.height}).first->second.lineHeight();
}

const FontGlyph& FontCache::getGlyph(unsigned size, char32_t ch) {
    const auto fit = m_glyphs.find(size);
    if (fit != m_glyphs.end()) {
        const auto fit2 = fit->second.map().find(ch);
        if (fit2 != fit->second.map().end())
            return fit2->second;
    }
    return cacheGlyph(size, ch);
}

void FontCache::rerenderTexture(SDL_Renderer * renderer) {
    ::std::vector<FontGlyph*> glyphs_to_render;
    for (auto& sized_cache : m_glyphs)
    for (auto& glyph_pair : sized_cache.second.map()) {
        FontGlyph& glyph = glyph_pair.second;
        if (glyph.needPrepare())
            glyph.prepare();
        if (glyph.ready())
            glyphs_to_render.push_back(&glyph);
    }

    if (glyphs_to_render.empty()) {
        m_texture = nullptr; // Nothing to render
        return;
    }

    const unsigned texSize = organizeGlyphs(::std::max(m_texSize, 1u), glyphs_to_render);

    // Create data
    ::std::unique_ptr<uint32_t[]> rgba32_data{new (::std::nothrow) uint32_t[texSize * texSize]};
    if (!rgba32_data) {
        printfe("Couldn't allocate %Iu bytes of memory.", texSize * texSize * sizeof(uint32_t));
        return;
    }

    if (texSize != m_texSize)
        if (!recreateTexture(renderer, texSize))
            return;

    for (unsigned i = 0; i < texSize*texSize; ++i)
        rgba32_data[i] = 0xB803FFFF;

    const int dst_pith = texSize * sizeof(uint32_t);

    bool succeded = true;
    for (FontGlyph* glyph : glyphs_to_render) {
        const FT_Bitmap & glyph_bitmap = glyph->bitmap();
        if (glyph_bitmap.width != glyph->texOff().w || glyph_bitmap.rows != glyph->texOff().h) {
            putse("Glyph texture offset and bitmap sizes doesn't match.");
            glyph->markNonExistent();
            succeded = false;
            continue;
        }

        uint32_t * dst = &rgba32_data[glyph->texOff().x + glyph->texOff().y * texSize];

        if (!ft_bitmap_rgba32_render(glyph_bitmap, dst, dst_pith)) {
            putse("Glyph rendering failed");
            glyph->markNonExistent();
            succeded = false;
            continue;
        }

        glyph->markRendered();
    }

    if (SDL_UpdateTexture(m_texture.get(), nullptr, rgba32_data.get(), dst_pith) != 0) {
        printfe("Couldn't update texture: %s", SDL_GetError());
    }

    m_isActual = succeded; // If rendering of some glyphs failed then texture will be regenerated without them
}

bool FontCache::recreateTexture(SDL_Renderer * renderer, unsigned texSize) noexcept {
    SDL_Texture_uptr texture{SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, texSize, texSize)};
    if (!texture) {
        printfe("Couldn't create %u^2 RGBA32 texture: %s", texSize, SDL_GetError());
        return false;
    }

    if (SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND) != 0) {
        printfe("Couldn't set RGBA32 texture blend mode: %s", SDL_GetError());
        return false;
    }

    m_texture = std::move(texture);
    m_texSize = texSize;

    return true;
}

const FontGlyph& FontCache::cacheGlyph(unsigned size, char32_t ch) {
    FontGlyph result;

    FT_Error error;
    if ((error = FT_Set_Char_Size(m_face, (FT_F26Dot6)(size << 6), (FT_F26Dot6)(size << 6), 96, 96)) != 0)
        printfe("Setting character size (%u pt) failed: %s.", size, ft_error_to_string(error));
    else {
        result.init(m_face, ch);
        if (result.hasGraphic())
            m_isActual = false;
    }

    const auto sfm_it = m_glyphs.find(size);
    if (sfm_it != m_glyphs.end())
        return sfm_it->second.map().emplace(ch, ::std::move(result)).first->second;

    return m_glyphs.emplace(size, SizedFontMap{m_face->size->metrics.height, ch, ::std::move(result)}).first->second.map().cbegin()->second;
}