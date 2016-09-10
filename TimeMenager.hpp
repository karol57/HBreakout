//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_TIMEMENAGER_HPP
#define SDLTEST_TIMEMENAGER_HPP

#include "WIN_TimeMenager.hpp"

class TimeMenager {
    public:
        void frame() noexcept;
        constexpr double delta() const noexcept { return m_delta; }
    private:
        WIN_TimeMenager m_native;
        double m_last;
        double m_delta;
        double m_current;
};

#endif // !SDLTEST_TIMEMENAGER_HPP