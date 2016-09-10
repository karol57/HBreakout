//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_BOUNDS_HPP
#define SDLTEST_BOUNDS_HPP

#include <SDL_render.h>
#include "Vector.hpp"
#include "GameInterfaces.hpp"

// Level (74x74 tiles) bounds
class Bounds : public IRenderable, public IStaticCollider {
public:
    Bounds(SDL_Texture * texture);
    virtual ~Bounds();

    virtual void checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                                Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                                ICollider *& col_item) noexcept override;
    virtual void renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept override;

    virtual void render(SDL_Renderer * renderer) const noexcept override;

    void switchSafe() noexcept { m_safe = !m_safe; }
private:
    SDL_Texture * m_texture;
    bool m_safe;
};

#endif // !SDLTEST_BOUNDS_HPP