//
// Created by Karol on 08.09.2016.
//

#include "TimeMenager.hpp"

void TimeMenager::frame() noexcept {
    m_native.frame();
    m_last = 1000.0 * (double)m_native.last() / (double)m_native.perfFreq;
    m_delta = 1000.0 * (double)m_native.delta() / (double)m_native.perfFreq;
    m_current = 1000.0 * (double)m_native.current() / (double)m_native.perfFreq;
}