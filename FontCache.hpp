//
// Created by Karol on 18.08.2016.
//

#ifndef SDLTEST_FONTCACHE_HPP
#define SDLTEST_FONTCACHE_HPP

#include <map>

#include <ft2build.h>
#include FT_TYPES_H
#include FT_GLYPH_H

#include <SDL.h>

#include "SDL_memory.hpp"
#include "FontGlyph.hpp"

class SizedFontMap {
    public:
        SizedFontMap(int lineHeight);
        SizedFontMap(int lineHeight, char32_t fch, FontGlyph && ffg);
        SizedFontMap(const SizedFontMap&) = delete;
        SizedFontMap(SizedFontMap&&);
        ~SizedFontMap();

        SizedFontMap& operator=(const SizedFontMap&) = delete;
        SizedFontMap& operator=(SizedFontMap&&);

        ::std::map<char32_t, FontGlyph>& map() noexcept { return m_map; }
        const ::std::map<char32_t, FontGlyph>& map() const noexcept { return m_map; }

        int lineHeight() const noexcept { return m_lineHeight; }
    private:
        ::std::map<char32_t, FontGlyph> m_map;
        int m_lineHeight;
};

class FontCache {
    public:
        FontCache(FT_Face face);
        FontCache(const FontCache&) = delete;
        FontCache(FontCache&&);
        ~FontCache();

        FontCache& operator=(const FontCache&) = delete;
        FontCache& operator=(FontCache&&);

        FT_Face face() { return m_face; }

        int lineHeight(unsigned size);
        const FontGlyph& getGlyph(unsigned size, char32_t ch);

        bool isActual() { return m_isActual; }
        SDL_Texture * texture() { return m_texture.get(); };
        unsigned texSize() const noexcept { return m_texSize; }

        void rerenderTexture(SDL_Renderer * renderer);
    private:
        FT_Face m_face;

        ::std::map<unsigned, SizedFontMap> m_glyphs; // size --> char32_t --> Glyph

        bool m_isActual;
        SDL_Texture_uptr m_texture;
        unsigned m_texSize;

        bool recreateTexture(SDL_Renderer * renderer, unsigned texSize) noexcept;

        const FontGlyph& cacheGlyph(unsigned size, char32_t ch);
};

#endif // !SDLTEST_FONTCACHE_HPP