//
// Created by Karol on 15.08.2016.
//

#ifndef SDLTEST_WIN_TIMEMENAGER_HPP
#define SDLTEST_WIN_TIMEMENAGER_HPP

#include <cstdint>
#include <Windows.h>

class WIN_TimeMenager {
    public:
        void frame() noexcept;

        int64_t last() const noexcept { return m_last; }
        int64_t delta() const noexcept { return m_delta; }
        int64_t current() const noexcept { return m_current; }

        static const int64_t perfFreq;
        static const int64_t start;
    private:
        int64_t m_last;
        int64_t m_delta;
        int64_t m_current;
};

#endif // !SDLTEST_WIN_TIMEMENAGER_HPP