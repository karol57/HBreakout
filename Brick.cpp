//
// Created by Karol on 08.09.2016.
//

#include "Brick.hpp"

Brick::Brick(unsigned x, unsigned y, unsigned width, unsigned height, SDL_Texture * texture) noexcept :
        m_texture{texture}, m_pos{x, y}, m_width{width}, m_height{height} {
    m_tl = (Vector2Dd)m_pos;
    m_br = (Vector2Dd)(m_pos + Vector2Du(m_width, m_height));
}
Brick::Brick(Brick&&) noexcept = default;
Brick::~Brick() = default;

Brick& Brick::operator=(Brick&&) noexcept = default;

void Brick::render(SDL_Renderer * renderer) const noexcept {
    const SDL_Rect rc { (int)(8u * m_pos.x), (int)(8u * m_pos.y),
                        (int)(8u * m_width), (int)(8u * m_height) };
    SDL_RenderCopy(renderer, m_texture, nullptr, &rc);
}

void Brick::on_collide() noexcept {
    m_destroyed = true;
}

void Brick::checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                           Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                           ICollider *& col_item) noexcept {
    if (!m_destroyed)
        StaticSquareCollider::checkCollision(proj_from, proj_to, proj_radius, col_pos, col_distSQ, col_norm, col_item);
}