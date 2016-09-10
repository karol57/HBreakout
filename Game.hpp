//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_GAME_HPP
#define SDLTEST_GAME_HPP

#include <vector>
#include <memory>

#include <SDL_render.h>
#include <SDL_events.h>

#include "SDL_memory.hpp"

#include "GameInterfaces.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "Bounds.hpp"
#include "Paddle.hpp"
#include "FontMenager.hpp"

// Game area is be 74x74 tiles (8x8 px on 800x600 resolution)
class Game {
    public:
        void init(SDL_Renderer * renderer);
        void update(double dt);
        void render(SDL_Renderer * renderer, FontMenager & fnt_mgr, unsigned fid);

        void on_key_up(SDL_KeyboardEvent& e);
        void on_key_down(SDL_KeyboardEvent& e);
        void on_mouse_wheel(SDL_MouseWheelEvent& e);
    private:
        bool m_showCollisions{false};

        unsigned m_score{0};
        unsigned m_levels{0};
        unsigned m_losts{0};

        SDL_Texture_uptr m_brickTexs[8]; // Brick textures
        SDL_Texture_uptr m_ballTexs[1];  // Ball textures
        SDL_Texture_uptr m_paddTexs[3];  // Paddle textures

        ::std::vector<::std::unique_ptr<Brick>> m_level;
        void createLevel();
        ::std::unique_ptr<Bounds> m_bounds;
        ::std::unique_ptr<Ball> m_ball;
        ::std::unique_ptr<Paddle> m_paddle;

        ::std::vector<IUpdateable*> m_moveable;
        ::std::vector<IRenderable*> m_rendereable;
        ::std::vector<ICollider*> m_collidable;
};


#endif // !SDLTEST_GAME_HPP