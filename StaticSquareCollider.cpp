//
// Created by Karol on 08.09.2016.
//

#include "intersection2D.hpp"
#include "SDL_helpers.hpp"
#include "StaticSquareCollider.hpp"

StaticSquareCollider::~StaticSquareCollider() = default;

void StaticSquareCollider::checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                                          Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                                          ICollider *& col_item) noexcept {
    // So, for physics ball is a infinite small point with want travel from
    // (bx;by) to (bnx;bny)
    // Because ball really isn't infinite small and have radius (br) then we must approximate simple rectangle model of
    // paddle by using 4 lines(sides) and 4 circles(vertices) moved away by radius from paddle

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

    auto testCircleCollision = [proj_from, proj_to, proj_radius, checkCollision](Vector2Dd cpos) {
        Vector2Dd inter[2];
        switch (line_segment_circle_intersection(proj_from.x, proj_from.y, proj_to.x, proj_to.y, cpos.x, cpos.y, proj_radius, inter[0].x, inter[0].y, inter[1].x, inter[1].y)) {
            case 2: checkCollision(inter[1], (inter[1] - cpos).normalized());
            case 1: checkCollision(inter[0], (inter[0] - cpos).normalized()); break;
            default:;
        }
    };

    const double left = m_tl.x - proj_radius;
    const double top = m_tl.y - proj_radius;
    const double right = m_br.x + proj_radius;
    const double bottom = m_br.y + proj_radius;

    const Vector2Dd diff = proj_to - proj_from;
    const bool from_left = diff.x > 0.0;
    const bool from_top = diff.y > 0.0;
    const bool from_right = diff.x < 0.0;
    const bool from_bottom = diff.y < 0.0;

    if (from_left)   testLineCollision(  left, m_tl.y,   left, m_br.y, { -1.0,  0.0 }); // Left line collision check
    if (from_top)    testLineCollision(m_tl.x,    top, m_br.x,    top, {  0.0, -1.0 }); // Top line collision check
    if (from_right)  testLineCollision( right, m_tl.y,  right, m_br.y, {  1.0,  0.0 }); // Right line collision check
    if (from_bottom) testLineCollision(m_tl.x, bottom, m_br.x, bottom, {  0.0,  1.0 }); // Bottom line collision check

    if (from_left  || from_top   ) testCircleCollision({ m_tl.x, m_tl.y }); // Top left circle collision check
    if (from_left  || from_bottom) testCircleCollision({ m_tl.x, m_br.y }); // Bottom left circle collision check
    if (from_right || from_top   ) testCircleCollision({ m_br.x, m_tl.y }); // Top right circle collision check
    if (from_right || from_bottom) testCircleCollision({ m_br.x, m_br.y }); // Bottom right circle collision check
}

void StaticSquareCollider::renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept {
    const Vector2Dd tl_pxd{8u * m_tl};
    const Vector2Dd br_pxd{8u * m_br};
    const Vector2Di tl_px(tl_pxd + 0.5); // Top left point in pixels
    const Vector2Di br_px(br_pxd + 0.5); // Bottom right point in pixels

    const Vector2Di tle_px{8u * (m_tl - proj_radius) + 0.5};
    const Vector2Di bre_px{8u * (m_br + proj_radius) + 0.5};

    SDL_RenderDrawLine(renderer, tle_px.x, tl_px.y, tle_px.x, br_px.y);
    SDL_RenderDrawLine(renderer, tl_px.x, tle_px.y, br_px.x, tle_px.y);
    SDL_RenderDrawLine(renderer, bre_px.x, tl_px.y, bre_px.x, br_px.y);
    SDL_RenderDrawLine(renderer, tl_px.x, bre_px.y, br_px.x, bre_px.y);

    SDL_RenderDrawCircle(renderer, tl_pxd.x, tl_pxd.y, 8u * proj_radius);
    SDL_RenderDrawCircle(renderer, tl_pxd.x, br_pxd.y, 8u * proj_radius);
    SDL_RenderDrawCircle(renderer, br_pxd.x, tl_pxd.y, 8u * proj_radius);
    SDL_RenderDrawCircle(renderer, br_pxd.x, br_pxd.y, 8u * proj_radius);
}