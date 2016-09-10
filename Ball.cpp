//
// Created by Karol on 08.09.2016.
//

#include <limits>
#include "Ball.hpp"

Ball::Ball(Vector2Dd pos, unsigned size, SDL_Texture * texture) :
        m_texture{texture}, m_size{size},
        m_enabled{false}, m_pos{pos}, m_vel{Vector2Dd::zero()}, m_lastCollided{nullptr} {}

Ball::Ball(Vector2Dd pos, unsigned size, Vector2Dd vel, SDL_Texture * texture) :
        m_texture{texture}, m_size{size},
        m_enabled{true}, m_pos{pos}, m_vel{vel}, m_lastCollided{nullptr} {}
Ball::~Ball() = default;

void Ball::update(double dt) noexcept {
    const Vector2Dd diff = m_vel*dt;
    m_nd = diff.lenSQ();
    m_npos = m_pos + diff;
}
void Ball::render(SDL_Renderer * renderer) const noexcept {
    const Vector2Dd rpos = 8u * m_pos - (8u*m_size / 2u);
    const SDL_Rect rc{ (int)(rpos.x+0.5), (int)(rpos.y+0.5), (int)(m_size * 8u), (int)(m_size * 8u) };
    SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
}

void Ball::checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                          Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                          ICollider *& col_item) noexcept {
    // TODO: Implement ball-ball collision
}

void Ball::renderPhysics(SDL_Renderer * renderer, double) const noexcept {
    const Vector2Di pos{ (Vector2Di)(8u * m_pos + 0.5) };
    SDL_RenderDrawPoint(renderer, pos.x, pos.y);
}

void Ball::updateCollision(double dt, std::vector<ICollider *> & colObjects) noexcept {
    if (!m_enabled)
        return;

    Vector2Dd col_pos;
    Vector2Dd col_norm;
    double fdcsq = ::std::numeric_limits<double>::max();
    ICollider * citem = nullptr;

    for (ICollider* collider : colObjects)
        if (collider != (ICollider*)this && collider != m_lastCollided)
            collider->checkCollision(m_pos, m_npos, m_size / 2.0, col_pos, fdcsq, col_norm, citem);

    if (citem) {
        m_pos = col_pos;
        m_lastCollided = citem;

        const double _2vn = 2u * m_vel.dot(col_norm);
        m_vel -= _2vn * col_norm;

        dt *= 1.0 - fdcsq / m_nd;

        citem->on_collide();
        update(dt);
        updateCollision(dt, colObjects);
    }
    else {
        m_pos = m_npos;
        m_lastCollided = nullptr;
    }
}