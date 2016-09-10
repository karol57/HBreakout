//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_COLLISIONTESTER_HPP
#define SDLTEST_COLLISIONTESTER_HPP

#include <vector>

#include <SDL_events.h>
#include <SDL_render.h>

#include "Vector.hpp"

class CollisionTester {
    public:
        struct Line { Vector2Di p1, p2; };
        struct Circle{ Vector2Di p; double r; };

        void render(SDL_Renderer* renderer);

        void on_mouse_button(SDL_MouseButtonEvent& e);
        void on_mouse_motion(SDL_MouseMotionEvent& e);
    private:
        ::std::vector<Line> m_lines;
        ::std::vector<Circle> m_circles;

        bool lineDrawing{false};
        Line m_curLine;

        bool circleDrawing{false};
        Circle m_curCircle;
};

#endif // !SDLTEST_COLLISIONTESTER_HPP