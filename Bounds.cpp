//
// Created by Karol on 08.09.2016.
//

#include "intersection2D.hpp"
#include "Bounds.hpp"

Bounds::Bounds(SDL_Texture * texture) : m_texture{texture}, m_safe{false} {}
Bounds::~Bounds() = default;

void Bounds::checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                            Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                            ICollider *& col_item) noexcept {
    auto checkCollision = [proj_from, &col_pos, &col_norm, &col_distSQ, &col_item, this](Vector2Dd inter, Vector2Dd norm) {
        const double dcsq = Vector2Dd::distSQ(proj_from, inter);
        if (dcsq < col_distSQ) {
            col_pos = inter;
            col_distSQ = dcsq;
            col_item = (ICollider*)this;
            col_norm = norm;
        }
    };

    auto testLineCollision = [proj_from, proj_to, checkCollision](double x1, double y1, double x2, double y2, Vector2Dd norm){
        Vector2Dd inter;
        if (line_segment_intersection(proj_from.x, proj_from.y, proj_to.x, proj_to.y, x1, y1, x2, y2, inter.x, inter.y))
            checkCollision(inter, norm);
    };

    // 4 lines
    // 1: (x1,y1), (x2,y1) // TOP
    // 2: (x1,y1), (x1,y2) // LEFT
    // 3: (x2,y1), (x2,y2) // RIGHT
    // 4: (x1,y2), (x2,y2) // BOTTOM

    //  (2;2) _ ____ _ (72;2)
    //         |    |
    // (2;72) _|____|_ (72;72)

    const double left = 2.0 + proj_radius;
    const double top = 2.0 + proj_radius;
    const double right = 72.0 - proj_radius;
    const double bottom = 72.0 - proj_radius;

    const Vector2Dd diff = proj_to - proj_from;
    const bool from_left = diff.x > 0.0;
    const bool from_top = diff.y > 0.0;
    const bool from_right = diff.x < 0.0;
    const bool from_bottom = diff.y < 0.0;

    if (from_right)  testLineCollision( left,    top,  left, bottom, {  1.0,  0.0 }); // Left border
    if (from_bottom) testLineCollision( left,    top, right,    top, {  0.0,  1.0 }); // Top border
    if (from_left)   testLineCollision(right,    top, right, bottom, { -1.0,  0.0 }); // Right border

    if (m_safe && from_top)    testLineCollision( left, bottom, right, bottom, {  0.0, -1.0 }); // Bottom border
}

void Bounds::renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept {
    const int left = (int)(8u * (2.0 + proj_radius) + 0.5);
    const int top = (int)(8u * (2.0 + proj_radius) + 0.5);
    const int right = (int)(8u * (72.0 - proj_radius) + 0.5);
    const int bottom = (int)(8u * (72.0 - proj_radius) + 0.5);

    if (m_safe) {
        SDL_Point lines[] = { { left, top }, { right, top }, { right, bottom }, { left, bottom }, { left, top } };
        SDL_RenderDrawLines(renderer, lines, 5);
    }
    else {
        SDL_Point lines[] = {{right, bottom},
                             {right, top},
                             {left,  top},
                             {left,  bottom}};
        SDL_RenderDrawLines(renderer, lines, 4);
    }
}

void Bounds::render(SDL_Renderer * renderer) const noexcept {
    SDL_Rect rc { 0, 0, 16, 16 };

    // Draw left border
    do {
        SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
        rc.y += 16;
    } while (rc.y < 592);

    // Draw right border
    rc.x = 576;
    rc.y = 0;
    do {
        SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
        rc.y += 16;
    } while (rc.y < 592);

    // Draw top border
    rc.x = 16;
    rc.y = 0;
    do {
        SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
        rc.x += 16;
    } while (rc.x < 576);

    // Draw bottom border
    if (m_safe) {
        rc.x = 16;
        rc.y = 576;
        do {
            SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
            rc.x += 16;
        } while (rc.x < 576);
    }
}