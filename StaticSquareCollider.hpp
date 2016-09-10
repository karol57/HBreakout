//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_STATICSQUARECOLLIDER_HPP
#define SDLTEST_STATICSQUARECOLLIDER_HPP

#include <SDL_render.h>
#include "Vector.hpp"
#include "GameInterfaces.hpp"

class StaticSquareCollider : public IStaticCollider {
    public:
        virtual ~StaticSquareCollider();

        virtual void
        checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                       Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                       ICollider *& col_item) noexcept override;
        virtual void renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept override;
    protected:
        Vector2Dd m_tl; // Top left corner of an rect
        Vector2Dd m_br; // Bottom right corner of an rect
};

#endif // !SDLTEST_STATICSQUARECOLLIDER_HPP