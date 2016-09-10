//
// Created by Karol on 08.09.2016.
//

#include "intersection2D.hpp"
#include "SDL_helpers.hpp"
#include "CollisionTester.hpp"

void CollisionTester::on_mouse_button(SDL_MouseButtonEvent & e) {
    if (e.state == SDL_PRESSED) {
        switch (e.button) {
            case SDL_BUTTON_LEFT:
                lineDrawing = true;
                m_curLine.p1 = m_curLine.p2 = Vector<Sint32, 2u>{e.x, e.y};
                break;
            case SDL_BUTTON_RIGHT:
                circleDrawing = true;
                m_curCircle.p = Vector<Sint32, 2u>{e.x, e.y};
                m_curCircle.r = 0.0;
                break;
            default:;
        }
    }
    else if (e.state == SDL_RELEASED) {
        switch (e.button) {
            case SDL_BUTTON_LEFT:
                lineDrawing = false;
                m_curLine.p2 = Vector<Sint32, 2u>{e.x, e.y};

                if (m_curLine.p1 != m_curLine.p2)
                    m_lines.push_back(m_curLine);
                break;
            case SDL_BUTTON_RIGHT:
                circleDrawing = false;
                m_curCircle.r = Vector2Di::dist(m_curCircle.p, Vector2Di{e.x, e.y});
                if (m_curCircle.r >= 1.0)
                    m_circles.push_back(m_curCircle);
                break;
            default:;
        }
    }
}
void CollisionTester::on_mouse_motion(SDL_MouseMotionEvent & e) {
    if ((e.state & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK)
        m_curLine.p2 = Vector<Sint32, 2u>{e.x, e.y};

    if ((e.state & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK)
        m_curCircle.r = Vector2Di::dist(m_curCircle.p, Vector2Di{e.x, e.y});
}

void CollisionTester::render(SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (const Line& line : m_lines)
        SDL_RenderDrawLine(renderer, line.p1.x, line.p1.y, line.p2.x, line.p2.y);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    if (lineDrawing)
        SDL_RenderDrawLine(renderer, m_curLine.p1.x, m_curLine.p1.y, m_curLine.p2.x, m_curLine.p2.y);

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
    for (const Circle& circle : m_circles)
        SDL_RenderDrawCircle(renderer, circle.p.x, circle.p.y, circle.r);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    if (circleDrawing)
        SDL_RenderDrawCircle(renderer, m_curCircle.p.x, m_curCircle.p.y, m_curCircle.r);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    auto collisionNormal = [renderer](Vector2Dd center, Vector2Dd intersection){
        const Vector2Dd norm{(intersection - center).normalized()};
        const Vector2Di p1{intersection + 0.5};
        const Vector2Di p2{intersection + 50.0 * norm + 0.5};

        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    };

    if (!m_lines.empty() && !m_circles.empty())
        for (const Line& line : m_lines)
            for (const Circle& circle : m_circles){
                Vector2Dd inter[2];
                switch (line_segment_circle_intersection(line.p1.x, line.p1.y, line.p2.x, line.p2.y,
                                                         circle.p.x, circle.p.y, circle.r,
                                                         inter[0].x, inter[0].y, inter[1].x, inter[1].y)) {
                    case 2:
                        SDL_RenderDrawCircle(renderer, inter[1].x, inter[1].y, 4);
                        collisionNormal((Vector2Dd)circle.p, inter[1]);
                    case 1:
                        SDL_RenderDrawCircle(renderer, inter[0].x, inter[0].y, 4);
                        collisionNormal((Vector2Dd)circle.p, inter[0]);
                        break;
                    default:;
                }
            }

    if (m_lines.size() >= 2)
        for (auto it1 = m_lines.cbegin(); it1 != (m_lines.cend() - 1); ++it1)
            for (auto it2 = it1; it2 != m_lines.cend(); ++it2) {
                const Line& l1 = *it1;
                const Line& l2 = *it2;
                Vector2Dd inter;
                if (line_segment_intersection(l1.p1.x, l1.p1.y, l1.p2.x, l1.p2.y, l2.p1.x, l2.p1.y, l2.p2.x, l2.p2.y, inter.x, inter.y))
                    SDL_RenderDrawCircle(renderer, inter.x, inter.y, 4);
            }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}