#include <cstdlib>
#include <ctime>

#include <limits>

#include <SDL.h>
#include <SDL_image.h>
#include "SDL_memory.hpp"

#include "error_log.hpp"

#include "TimeMenager.hpp"
#include "FontMenager.hpp"
#include "FPSWidget.hpp"

#include "Game.hpp"
#include "CollisionTester.hpp"

class DebugTextures {
    public:
        bool recreate(SDL_Renderer * renderer);
        void renderBG(unsigned id, SDL_Renderer * renderer);

        unsigned width = 800;
        unsigned height = 600;

        SDL_Texture_uptr bgBWTexture;
        SDL_Texture_uptr bgGGTexture;
        SDL_Texture_uptr bgCLTexture;
};
bool DebugTextures::recreate(SDL_Renderer * renderer) {
    static constexpr
    uint32_t bg_BWTexture[2][2] = { { 0x000000FF, 0xFFFFFFFF },
                                    { 0xFFFFFFFF, 0x000000FF } };
    bgBWTexture = SDL_Texture_uptr{ SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 2, 2) };
    if (!bgBWTexture)
        printfe("Couldn't create BW texture: %s.", SDL_GetError());
    else if (SDL_UpdateTexture(bgBWTexture.get(), nullptr, bg_BWTexture, 2 * sizeof(uint32_t)) != 0) {
        printfe("Couldn't set BW texture data: %s.", SDL_GetError());
        bgBWTexture = { nullptr };
    }

    static constexpr
    uint32_t bg_GGTexture[2][2] = { { 0x666666FF, 0x999999FF },
                                    { 0x999999FF, 0x666666FF }};
    bgGGTexture = SDL_Texture_uptr{ SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 2, 2) };
    if (!bgGGTexture)
        printfe("Couldn't create GG texture: %s.", SDL_GetError());
    else if (SDL_UpdateTexture(bgGGTexture.get(), nullptr, bg_GGTexture, 2 * sizeof(uint32_t)) != 0) {
        printfe("Couldn't set GG texture data: %s.", SDL_GetError());
        bgGGTexture = { nullptr };
    }

    static constexpr
    uint32_t bg_CLTexture[5][5] = { { 0x008080FF, 0x0000FFFF, 0x000000FF, 0xC0C0C0FF, 0xFF0000FF },
                                    { 0x0000FFFF, 0x000000FF, 0xC0C0C0FF, 0xFF0000FF, 0x008080FF },
                                    { 0x000000FF, 0xC0C0C0FF, 0xFF0000FF, 0x008080FF, 0x0000FFFF },
                                    { 0xC0C0C0FF, 0xFF0000FF, 0x008080FF, 0x0000FFFF, 0x000000FF },
                                    { 0xFF0000FF, 0x008080FF, 0x0000FFFF, 0x000000FF, 0xC0C0C0FF } };
    bgCLTexture = SDL_Texture_uptr{ SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 5, 5) };
    if (!bgCLTexture)
        printfe("Couldn't create GG texture: %s.", SDL_GetError());
    else if (SDL_UpdateTexture(bgCLTexture.get(), nullptr, bg_CLTexture, 5 * sizeof(uint32_t)) != 0) {
        printfe("Couldn't set GG texture data: %s.", SDL_GetError());
        bgCLTexture = { nullptr };
    }
}
void DebugTextures::renderBG(unsigned id, SDL_Renderer * renderer) {
    id %= 4;

    SDL_Texture * tex = [this](unsigned id) -> SDL_Texture* {
        switch (id) {
            case 1: return bgBWTexture.get();
            case 2: return bgGGTexture.get();
            case 3: return bgCLTexture.get();
            default:;
        };
        return nullptr;
    }(id);

    if (!tex)
        SDL_RenderClear(renderer);
    else {
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

        const SDL_Rect src = { 0, 0, w, h };
        SDL_Rect dst = { 0, 0, w*10, h*10 };

        for (;;) {
            SDL_RenderCopy(renderer, tex, &src, &dst);

            if (dst.x + dst.w > width) {
                if (dst.y + dst.h > height)
                    break;
                dst.y += dst.h;
                dst.x = 0;
            }
            else
                dst.x += dst.w;
        }
    }

}

extern C_LINKAGE
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printfe("Unable to initialize SDL: %s", SDL_GetError());
        return STEC_SDL_INIT;
    }
    atexit(SDL_Quit);

    constexpr int IMG_Init_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
    if (IMG_Init(IMG_Init_flags) != IMG_Init_flags) {
        printfe("Unable to initialize SDL_image: %s", IMG_GetError());
        return STEC_SDL_IMAGE_INIT;
    }
    atexit(IMG_Quit);

    srand((unsigned)time(NULL));
    FontMenager fnt_mng;

    const unsigned dbg_fid = fnt_mng.loadFont("../fonts/unifont.ttf");
    if (dbg_fid == ::std::numeric_limits<unsigned>::max())
        return STEC_TTF_DBG_LOAD;

    constexpr int rt_width = 800;
    constexpr int rt_height = 600;

    SDL_Window_uptr main_window{SDL_CreateWindow("HBreakout v0.0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, rt_width, rt_height, SDL_WINDOW_SHOWN)};
    if (!main_window) {
        printfe("Main window could not be created! SDL_Error: %s", SDL_GetError());
        return STEC_SDL_WIN;
    }

    SDL_Renderer_uptr main_renderer{SDL_CreateRenderer(main_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE)};
    if (!main_renderer) {
        printfe("Main renderer could not be created! SDL_Error: %s", SDL_GetError());
        return STEC_SDL_REN;
    }

    DebugTextures dbgTexs;
    dbgTexs.recreate(main_renderer.get());
    SDL_SetRenderDrawColor(main_renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE); // Background color

    TimeMenager time_menager;
    time_menager.frame();

    FPSWidget fpsWidget;
    fpsWidget.init(main_renderer.get());

    Game game;
    game.init(main_renderer.get());

    CollisionTester tester;

    unsigned bgID = 0; // Background id
    bool bgEnabled = true; // Clear background
    bool showFontCache = false; // Debug show font cache
    bool showFPSMeter = false; // Debug FPS widget
    auto engine_on_key_down = [&bgID, &bgEnabled, &showFontCache, &showFPSMeter](SDL_KeyboardEvent & e) {
        switch (e.keysym.sym) {
            case SDLK_BACKQUOTE: bgEnabled = !bgEnabled; break;
            case SDLK_1: --bgID; break;
            case SDLK_2: ++bgID; break;
            case SDLK_3: showFontCache = !showFontCache; break;
            case SDLK_4: showFPSMeter = !showFPSMeter; break;
            default:;
        }
    };

    bool quit = !main_window;
    do {
        time_menager.frame();

        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT: quit = true; break;
                case SDL_KEYDOWN: engine_on_key_down(e.key);
                                  game.on_key_down(e.key); break;
                case SDL_KEYUP: game.on_key_up(e.key); break;
                case SDL_MOUSEMOTION: tester.on_mouse_motion(e.motion); break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: tester.on_mouse_button(e.button); break;
                case SDL_MOUSEWHEEL: game.on_mouse_wheel(e.wheel); break;
                default:;
            }
        }

        fpsWidget.update(time_menager.delta());
        game.update(time_menager.delta());

        if (bgEnabled)
            dbgTexs.renderBG(bgID, main_renderer.get());

        tester.render(main_renderer.get());
        game.render(main_renderer.get(), fnt_mng, dbg_fid);
        fnt_mng.render(main_renderer.get());

        if (showFontCache) {
            FontCache & f_cache = fnt_mng.getCache(dbg_fid);
            const SDL_Rect rc{rt_width - (int) f_cache.texSize() - 10, 10,
                              (int) f_cache.texSize(), (int) f_cache.texSize()};
            SDL_SetTextureColorMod(f_cache.texture(), 255u, 255u, 255u);
            SDL_RenderCopy(main_renderer.get(), f_cache.texture(), nullptr, &rc);
        }

        if (showFPSMeter) {
            fpsWidget.updateTexture(main_renderer.get(), fnt_mng, dbg_fid);
            const SDL_Rect rc = { (int)(rt_width - fpsWidget.width() - 10u), (int)(rt_height - fpsWidget.height() - 10u),
                                  (int)fpsWidget.width(), (int)fpsWidget.height() };
            SDL_RenderCopy(main_renderer.get(), fpsWidget.texture(), nullptr, &rc);
        }

        SDL_RenderPresent(main_renderer.get());
    } while (!quit);

    return STEC_SUCCESS;
}