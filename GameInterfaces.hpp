//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_GAMEINTERFACES_HPP
#define SDLTEST_GAMEINTERFACES_HPP

#include <vector>
#include <SDL_render.h>

#include "Vector.hpp"

class IUpdateable {
public:
    virtual ~IUpdateable();
    virtual void update(double dt) noexcept = 0;
};
class ICollider {
public:
    virtual ~ICollider();

    // Check collision with ball
    // (bx;by) - ball source position
    // (bnx;bny) - ball destination position
    // br - ball radius
    // nx - collided surface normal x
    // ny - collided surface normal y
    // cx - collision point
    // cy - collision point
    // fdcsq - distance from ball to collision point
    // citem - collided item
    virtual void checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                                Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                                ICollider *& col_item) noexcept = 0;
    virtual void updateCollision(double dt, ::std::vector<ICollider*> & colObjects) noexcept = 0;

    virtual void on_collide() noexcept;

    // Debug method showing collision shapes
    virtual void renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept = 0;
};
class IStaticCollider : public ICollider {
public:
    virtual ~IStaticCollider();
    virtual void updateCollision(double dt, std::vector<ICollider*>& colObjects) noexcept override;
};
class IRenderable {
public:
    virtual ~IRenderable();
    virtual void render(SDL_Renderer * renderer) const noexcept = 0;
};

#endif // !SDLTEST_GAMEINTERFACES_HPP