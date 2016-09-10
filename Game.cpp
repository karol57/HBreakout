//
// Created by Karol on 08.09.2016.
//

#include <SDL_image.h>
#include "Game.hpp"
#include "error_log.hpp"

static
SDL_Texture_uptr loadTexture(SDL_Renderer * renderer, const char * pattern, unsigned id) noexcept {
    constexpr unsigned assetPathBufferSize = 128;
    char file[assetPathBufferSize];
    const int print_res = snprintf(file, assetPathBufferSize, pattern, id);

    if (print_res < 0) {
        printfe("Couldn't generate texture path. Pattern: \"%s\", id: %u", pattern, id);
        return {nullptr};
    }

    if (print_res == 0)
        return {nullptr};

    if (print_res >= assetPathBufferSize) {
        printfe("Texture path too long. Pattern: \"%s\", id: %u", pattern, id);
        return {nullptr};
    }

    SDL_Surface_uptr surf{ IMG_Load(file) };
    if (!surf) {
        printfe("Couldn't load texture \"%s\": %s", file, IMG_GetError());
        return {nullptr};
    }

    SDL_Texture_uptr result{ SDL_CreateTextureFromSurface(renderer, surf.get()) };
    if (!result)
        printfe("Couldn't create texture \"%s\": %s", file, SDL_GetError());
    return std::move(result);
}

void Game::init(SDL_Renderer * renderer) {
    unsigned id = 0;
    for (SDL_Texture_uptr& brick : m_brickTexs)
        brick = loadTexture(renderer, "../images/brick%u.png", id++);

    id = 0;
    for (SDL_Texture_uptr& ball : m_ballTexs)
        ball = loadTexture(renderer, "../images/ball%u.png", id++);

    id = 0;
    for (SDL_Texture_uptr& paddChunk : m_paddTexs)
        paddChunk = loadTexture(renderer, "../images/padd%u.png", id++);

    m_bounds = std::make_unique<Bounds>(m_brickTexs[0].get());
    m_ball = std::make_unique<Ball>(Vector2Dd{ 36.0, 69.5 }, 1u, m_ballTexs[0].get());
    m_paddle = std::make_unique<Paddle>(36.0, 71.0, m_paddTexs[0].get(), m_paddTexs[1].get(), m_paddTexs[2].get());

    m_rendereable.emplace_back((IRenderable*)m_bounds.get());
    m_rendereable.emplace_back((IRenderable*)m_paddle.get());
    m_rendereable.emplace_back((IRenderable*)m_ball.get());

    m_moveable.emplace_back((IUpdateable*)m_ball.get());
    m_moveable.emplace_back((IUpdateable*)m_paddle.get());

    m_collidable.emplace_back((ICollider*)m_ball.get());
    m_collidable.emplace_back((ICollider*)m_paddle.get());
    m_collidable.emplace_back((ICollider*)m_bounds.get());
}

void Game::update(double dt) {
    for (IUpdateable * moveable : m_moveable)
        moveable->update(dt);

    for (ICollider * collider : m_collidable)
        collider->updateCollision(dt, m_collidable);

    const Vector2Dd& ballPos = m_ball->position();
    if (ballPos.x < 0.0 || ballPos.x > 74.0 || ballPos.y < 0.0 || ballPos.y > 74.0) {
        m_paddle->boundBall(*m_ball.get());
        ++m_losts;
    }

    for (auto it = m_level.cbegin(); it != m_level.cend();)
        if ((*it)->destroyed()) {
            const Brick * brick = it->get();

            for (auto it2 = m_rendereable.cbegin(); it2 != m_rendereable.cend(); ++it2)
                if (*it2 == (const IRenderable*)brick) {
                    m_rendereable.erase(it2);
                    break;
                }
            for (auto it2 = m_collidable.cbegin(); it2 != m_collidable.cend(); ++it2)
                if (*it2 == (const ICollider*)brick) {
                    m_collidable.erase(it2);
                    break;
                }

            it = m_level.erase(it);
            m_score += 25;
        }
        else
            ++it;

    if (m_level.empty()) {
        createLevel();
        ++m_levels;
        for (auto& brick : m_level)
            m_rendereable.emplace_back((IRenderable*)brick.get());
        for (auto& brick : m_level)
            m_collidable.emplace_back((ICollider*)brick.get());
        m_paddle->boundBall(*m_ball.get());
    }
}

void Game::render(SDL_Renderer * renderer, FontMenager & fnt_mgr, unsigned fid) {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    for (const IRenderable * renderable : m_rendereable)
        renderable->render(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
    if (m_showCollisions)
        for (const ICollider * collider : m_collidable)
            collider->renderPhysics(renderer, 0.5);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    constexpr size_t scoreBuff = 128u;
    char32_t scoreText[scoreBuff];
    size_t scoreLen;
    {
        char buffer[scoreBuff];
        int result = snprintf(buffer, scoreBuff, "Score: %u\nLevel: %u\nLosts: %u", m_score, m_levels, m_losts);
        if (result <= 0)
            scoreLen = 0u;
        else {
            scoreLen = (size_t)result;
            for (size_t i = 0u; i < scoreLen; ++i)
                scoreText[i] = (char32_t)buffer[i];
        }
    }
    fnt_mgr.drawText(renderer, {fid}, std::u32string(scoreText, scoreLen), 8*74+10, 10, VA_TOP | HA_LEFT);
}

void Game::on_key_up(SDL_KeyboardEvent & e) {
    if (e.repeat == 0)
        switch (e.keysym.sym) {
            case SDLK_LEFT:  m_paddle->incAccX(0.00025); break;
            case SDLK_RIGHT: m_paddle->decAccX(0.00025); break;

            case SDLK_UP:    m_paddle->incAccY(0.00025); break;
            case SDLK_DOWN:  m_paddle->decAccY(0.00025); break;
            case SDLK_5:     m_showCollisions = !m_showCollisions; break;
            default:;
        }
}

void Game::on_key_down(SDL_KeyboardEvent & e) {
    if (e.repeat == 0)
        switch (e.keysym.sym) {
            case SDLK_LEFT:     m_paddle->decAccX(0.00025); break;
            case SDLK_RIGHT:    m_paddle->incAccX(0.00025); break;

            case SDLK_UP:      m_paddle->decAccY(0.00025); break;
            case SDLK_DOWN:    m_paddle->incAccY(0.00025); break;

            case SDLK_SPACE:   m_paddle->fire(); break;

            case SDLK_6:       m_bounds->switchSafe(); break;
            default:;
        }
}

void Game::on_mouse_wheel(SDL_MouseWheelEvent & e) {
    if(e.y > 0)
        m_ball->speedUp();
    else if (e.y < 0)
        m_ball->slowDown();
}

void Game::createLevel() {
    for (unsigned x = 2u; x < 70u; x += 5u)
        for (unsigned y = 2u; y < 64u; y += 3u)
            if (rand() % (rand() % 10 + 1) == 0)
                m_level.emplace_back(::std::make_unique<Brick>(x, y, 4u, 2u, m_brickTexs[1+rand()%7].get()));
}