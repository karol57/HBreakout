//
// Created by Karol on 15.08.2016.
//

#include "WIN_TimeMenager.hpp"

static int64_t getPerformanceFrequency() noexcept {
    LARGE_INTEGER perfFrequency;

    QueryPerformanceFrequency(&perfFrequency);
    // MSDN: On systems that run Windows XP or later,
    //       the function will always succeed and will thus never return zero.

    return perfFrequency.QuadPart;
}
static int64_t getPerformanceCounter() noexcept {
    LARGE_INTEGER perfCounter;

    QueryPerformanceCounter(&perfCounter);
    // MSDN: On systems that run Windows XP or later,
    //       the function will always succeed and will thus never return zero.

    return perfCounter.QuadPart;
}

const int64_t WIN_TimeMenager::perfFreq = getPerformanceFrequency();
const int64_t WIN_TimeMenager::start = getPerformanceCounter();

void WIN_TimeMenager::frame() noexcept {
    const int64_t current = getPerformanceCounter();

    m_last = m_current;
    m_current = current - start;
    m_delta = m_current - m_last;
}