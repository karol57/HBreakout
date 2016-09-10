//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_PADDLE_HPP
#define SDLTEST_PADDLE_HPP

#include <SDL_render.h>
#include "Vector.hpp"
#include "GameInterfaces.hpp"
#include "StaticSquareCollider.hpp"
#include "Ball.hpp"

class Paddle : public IRenderable, public IUpdateable, public StaticSquareCollider {
public:
    Paddle(double x, double y, SDL_Texture * ltex, SDL_Texture * mtex, SDL_Texture * rtex) noexcept;
    virtual ~Paddle();

    virtual void update(double dt) noexcept override;
    virtual void render(SDL_Renderer * renderer) const noexcept override;

    void incAccX(double ax) noexcept { m_acc.x += ax; }
    void decAccX(double ax) noexcept { m_acc.x -= ax; }

    void incAccY(double ay) noexcept { m_acc.y += ay; }
    void decAccY(double ay) noexcept { m_acc.y -= ay; }

    void boundBall(Ball& ball) noexcept;
    void fire() noexcept;
private:
    void updateRect() noexcept;

    Ball * m_ball;
    double m_shootDir{ 0.0 };      // Direction of shoot [radians]
    double m_shootDirVel{ 0.005 };   // Velocity of direction [radians/ms]

    SDL_Texture * m_ltex; // Left segment texture
    SDL_Texture * m_mtex; // Middle segments texture
    SDL_Texture * m_rtex; // Right segment texture

    Vector2Dd m_pos;  // Paddle position (in tiles)
    Vector2Dd m_vel; // Paddle velocity (in tiles/ms)
    Vector2Dd m_acc; // Paddle acceleration (in tiles/ms^2)

    unsigned m_ps; // Paddle size (count of middle segments)
    unsigned m_phw; // Paddle half-width;
    unsigned m_phh; // Paddle half-height;
};

#endif // !SDLTEST_PADDLE_HPP