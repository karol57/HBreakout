//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_BALL_HPP
#define SDLTEST_BALL_HPP

#include <SDL_render.h>

#include "Vector.hpp"
#include "GameInterfaces.hpp"

class Ball : public IRenderable, public IUpdateable, public ICollider {
public:
    Ball(Vector2Dd pos, unsigned size, SDL_Texture * texture);
    Ball(Vector2Dd pos, unsigned size, Vector2Dd vel, SDL_Texture * texture);

    virtual ~Ball();

    virtual void update(double dt) noexcept override;
    virtual void render(SDL_Renderer * renderer) const noexcept override;

    virtual void updateCollision(double dt, std::vector<ICollider*>& colObjects) noexcept override;
    virtual void checkCollision(Vector2Dd proj_from, Vector2Dd proj_to, double proj_radius,
                                Vector2Dd & col_pos, double & col_distSQ, Vector2Dd & col_norm,
                                ICollider *& col_item) noexcept override;
    virtual void renderPhysics(SDL_Renderer * renderer, double proj_radius) const noexcept override;

    void setLastCollided(const ICollider * collider) noexcept { m_lastCollided = collider; }

    const Vector2Dd& position() const noexcept { return m_pos; }

    void setPosition(Vector2Dd pos) noexcept { m_pos = pos; }
    void setVelocity(Vector2Dd vel) noexcept { m_vel = vel; }

    void speedUp() noexcept { m_vel *= 2.0; }
    void slowDown() noexcept { m_vel *= 0.5; }

    void enableMotion() noexcept { m_enabled = true; }
    void disableMotion() noexcept { m_enabled = false; }

    double radius() const noexcept { return m_size / 2.0; }
private:
    SDL_Texture * m_texture;
    unsigned m_size; // Size (in tiles)

    bool m_enabled;

    Vector2Dd m_pos;  // Position (in tiles)
    Vector2Dd m_npos; // Next position (in tiles)
    double m_nd;      // Next position distance
    Vector2Dd m_vel;  // Velocity (in tiles/ms)

    const ICollider * m_lastCollided;
};

#endif // !SDLTEST_BALL_HPP