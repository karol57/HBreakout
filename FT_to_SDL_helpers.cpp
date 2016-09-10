//
// Created by Karol on 18.08.2016.
//

#include "FT_to_SDL_helpers.hpp"
#include "error_log.hpp"

static constexpr
void FT_BitmapGrayRenderer(const FT_Bitmap& src, uint32_t* dst_it, int dst_pith) {
    const uint8_t * src_it = static_cast<const uint8_t*>(src.buffer);
    for (unsigned y = 0; y < src.rows; ++y) {
        for (unsigned x = 0; x < src.width; ++x) {
            dst_it[x] = 0xFFFFFF00u | src_it[x];
        }
        src_it += src.pitch;
        dst_it = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(dst_it) + dst_pith);
    }
}

bool ft_bitmap_rgba32_render(const FT_Bitmap& src, uint32_t* dst, int dst_pith) {
    switch (src.pixel_mode) {
        case FT_PIXEL_MODE_NONE: putse("Bitmap with FT_PIXEL_MODE_NONE passed."); return false;
        case FT_PIXEL_MODE_GRAY: FT_BitmapGrayRenderer(src, dst, dst_pith); return true;

        case FT_PIXEL_MODE_MONO: // TODO: Add support for FT_PIXEL_MODE_MONO
        case FT_PIXEL_MODE_BGRA: // TODO: Add support for FT_PIXEL_MODE_BGRA
        case FT_PIXEL_MODE_GRAY2:
        case FT_PIXEL_MODE_GRAY4:
        case FT_PIXEL_MODE_LCD:
        case FT_PIXEL_MODE_LCD_V: printfe("Unsupported bitmap format: %hhu", src.pixel_mode); return false;
        default:;
    }

    printfe("Unknown bitmap format: %hhu", src.pixel_mode);
    return false;
}