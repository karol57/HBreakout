//
// Created by Karol on 08.09.2016.
//

#include "Paddle.hpp"

Paddle::Paddle(double x, double y, SDL_Texture * ltex, SDL_Texture * mtex, SDL_Texture * rtex) noexcept :
        m_ball{nullptr},
        m_ltex{ltex}, m_mtex{mtex}, m_rtex{rtex},

        m_pos{x, y},
        m_vel{Vector2Dd::zero()},
        m_acc{Vector2Dd::zero()},
        m_ps{4u}, m_phw{(2u + 2u*m_ps + 2u) / 2u}, m_phh{1u} {
    updateRect();
}
Paddle::~Paddle() = default;

void Paddle::update(double dt) noexcept {
    m_pos += m_vel * dt;

    constexpr double drag = -0.00725;
    m_vel += (m_acc + drag * m_vel) * dt;

    if (m_pos.x < 2.0+m_phw) {
        m_pos.x = 2.0+m_phw;
        m_vel.x = 0.0;
    }
    else if (m_pos.x > 72.0-m_phw) {
        m_pos.x = 72.0-m_phw;
        m_vel.x = 0.0;
    }

    if (m_pos.y < 2.0+m_phh) {
        m_pos.y = 2.0+m_phh;
        m_vel.y = 0.0;
    }
    else if (m_pos.y > 72-m_phh) {
        m_pos.y = 72.0-m_phh;
        m_vel.y = 0.0;
    }

    if (m_ball) {
        m_ball->setPosition({m_pos.x, m_pos.y - m_phh - m_ball->radius()});

        m_shootDir += m_shootDirVel * dt;
        if (m_shootDir < -M_PI) {
            m_shootDir = -M_PI;
            m_shootDirVel = -m_shootDirVel;
        } else if (m_shootDir > 0.0) {
            m_shootDir = 0.0;
            m_shootDirVel = -m_shootDirVel;
        }
    }

    updateRect();
}
void Paddle::render(SDL_Renderer * renderer) const noexcept {
    const unsigned pw = 8u * m_phw * 2u;
    const unsigned ph = 8u * m_phh * 2u;

    const Vector2Di pos{ 8u * m_pos - Vector2Du{pw / 2u, ph / 2u} + 0.5 }; // Paddle pos in px

    SDL_Rect rc{ pos.x, pos.y, 16, 16 };
    SDL_RenderCopy(renderer, m_ltex, nullptr, &rc);
    for (unsigned i = 0; i < m_ps; ++i) {
        rc.x += 16;
        SDL_RenderCopy(renderer, m_mtex, nullptr, &rc);
    }
    rc.x += 16;
    SDL_RenderCopy(renderer, m_rtex, nullptr, &rc);

    if (m_ball) {
        const Vector2Di ballPos = (Vector2Di)(8u * m_ball->position() + 0.5);
        const Vector2Di shootVec = (Vector2Di)(8u * (m_ball->position() + 2.5 * Vector2Dd{cos(m_shootDir), sin(m_shootDir)}) + 0.5);

        SDL_SetRenderDrawColor(renderer, 255, 0 ,0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, ballPos.x, ballPos.y, shootVec.x, shootVec.y);
    }
}

void Paddle::updateRect() noexcept {
    m_tl = m_pos - Vector2Du{m_phw, m_phh};
    m_br = m_pos + Vector2Du{m_phw, m_phh};
}

void Paddle::boundBall(Ball & ball) noexcept {
    ball.disableMotion();
    ball.setPosition({ m_pos.x + m_phw + ball.radius(), m_pos.y - m_phh - ball.radius() });
    m_ball = &ball;
}

void Paddle::fire() noexcept {
    if (m_ball) {
        m_ball->setLastCollided((ICollider*)this);
        m_ball->setVelocity(0.03125 * Vector2Dd{cos(m_shootDir), sin(m_shootDir)});
        m_ball->enableMotion();
        m_ball = nullptr;
    }
}