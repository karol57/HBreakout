//
// Created by Karol on 03.09.2016.
//

#ifndef SDLTEST_FPSWIDGET_HPP
#define SDLTEST_FPSWIDGET_HPP

#include <SDL_render.h>
#include "FontMenager.hpp"
#include "SDL_memory.hpp"
#include "BasicDTMenager.hpp"

class FPSWidget {
    public:
        FPSWidget();
        ~FPSWidget();

        bool init(SDL_Renderer * render) noexcept;
        void deinit() noexcept;

        void update(double dt) noexcept;
        void updateTexture(SDL_Renderer * renderer, FontMenager& fnt_mgr, unsigned fid) noexcept;
        SDL_Texture * texture() { return m_buffer.get(); }

        unsigned width() const noexcept { return m_width; }
        unsigned height() const noexcept { return m_height; }
    private:
        BasicDTMenager<double, 1024u> m_dt_mgr;

        double m_currMaxDT{1.0}; // Maximum delta time (ms) shown on graph
        double m_destMaxDT{1.0}; // Maximum delta time (ms) that should'be shown on graph
        double m_maxDTVel{0.0};  // Velcoty (ms/ms) - how fast m_currMaxDT will be going to m_destMaxDT

        unsigned m_width{256u+128u};  // Width of the widget
        unsigned m_height{128u}; // Height of the widget

        SDL_Texture_uptr m_buffer;
};

#endif // !SDLTEST_FPSWIDGET_HPP