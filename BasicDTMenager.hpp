//
// Created by Karol on 15.08.2016.
//

#ifndef SDLTEST_BASICDTMENAGER_HPP
#define SDLTEST_BASICDTMENAGER_HPP

#include <limits>
#include <algorithm>

template<typename dt_type = unsigned, unsigned _capacity = 1024u, typename avg_type = double>
class BasicDTMenager {
    public:
        static constexpr unsigned capacity{_capacity};

        constexpr unsigned frames() const noexcept { return m_frames; }
        constexpr const dt_type (&samples() const noexcept)[capacity] { return m_samples; }

        constexpr dt_type min() const noexcept { return m_min; }
        constexpr avg_type averange() const noexcept { return m_avg; }
        constexpr dt_type max() const noexcept { return m_max; }

        constexpr void update(dt_type dt) noexcept {
            m_samples[m_frames % capacity] = dt;
            ++m_frames;
            const unsigned buffer_size = std::min(m_frames, capacity);

            dt_type min{::std::numeric_limits<dt_type>::max()};
            dt_type sum{0};
            dt_type max{::std::numeric_limits<dt_type>::min()};

            for (dt_type* it = &m_samples[0]; it < &m_samples[buffer_size]; ++it) {
                const dt_type val = *it;
                sum += val;
                min = ::std::min(min, val);
                max = ::std::max(max, val);
            }

            m_min = min;
            m_avg = sum / avg_type(buffer_size);
            m_max = max;
        }
    private:
        unsigned m_frames = 0u;
        dt_type m_samples[capacity];

        dt_type m_min;
        avg_type m_avg;
        dt_type m_max;
};

template<typename dt_type, unsigned _capacity, typename avg_type>
constexpr unsigned BasicDTMenager<dt_type, _capacity, avg_type>::capacity;

#endif // !SDLTEST_BASICDTMENAGER_HPP