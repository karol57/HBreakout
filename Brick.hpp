//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_BRICK_HPP
#define SDLTEST_BRICK_HPP

#include <SDL_render.h>

#include "Vector.hpp"
#include "GameInterfaces.hpp"
#include "StaticSquareCollider.hpp"

class Brick : public IRenderable, public StaticSquareCollider {
public:
    Brick(unsigned x, unsigned y, unsigned width, unsigned height, SDL_Texture * texture) noexcept;
    Brick(Brick&&) noexcept;
    virtual ~Brick();

    Brick& operator=(Brick&&) noexcept;

    virtual void render(SDL_Renderer * renderer) const noexcept override;

    virtual void checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                                Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                                ICollider *& col_item) noexcept override;
    virtual void on_collide() noexcept override;

    bool destroyed() const noexcept { return m_destroyed; }
private:
    SDL_Texture * m_texture;

    Vector2Du m_pos; // Position (in tiles)
    unsigned m_width, m_height; // Size (in tiles)

    bool m_destroyed{false};
};

#endif // !SDLTEST_BRICK_HPP