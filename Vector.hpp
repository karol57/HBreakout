//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_VECTOR_HPP
#define SDLTEST_VECTOR_HPP

#include "VecCoords.hpp"
#include <type_traits>
#include "op_type.hpp"

template<typename T, unsigned D>
struct Vector : VecCoords<T, D> {
    // Helpers
    using adv_math_type = ::std::conditional_t<::std::is_floating_point<T>::value, T, double>;

    template<typename T2>
    using dot_type_t = sum_op_type_t<mul_op_type_t<T, T2>, mul_op_type_t<T, T2>>;

    template<typename>
    struct is_vector : std::false_type {};
    template<typename T2, unsigned D2>
    struct is_vector<Vector<T2,D2>> : std::true_type {};

    // Defaults
    constexpr Vector() noexcept = default;
    constexpr Vector(const Vector&) noexcept = default;
    constexpr Vector(Vector&&) noexcept = default;
    ~Vector() noexcept = default;

    constexpr Vector& operator=(const Vector&) noexcept = default;
    constexpr Vector& operator=(Vector&&) noexcept = default;

    // C-tors
    template<unsigned _D = D, typename = ::std::enable_if_t<_D == 1u>>
    constexpr Vector(T x) noexcept {
        this->x = x;
    }
    template<unsigned _D = D, typename = ::std::enable_if_t<_D == 2u>>
    constexpr Vector(T x, T y) noexcept {
        this->x = x;
        this->y = y;
    }
    template<unsigned _D = D, typename = ::std::enable_if_t<_D == 3u>>
    constexpr Vector(T x, T y, T z) noexcept {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    template<unsigned _D = D, typename = ::std::enable_if_t<_D == 4u>>
    constexpr Vector(T x, T y, T z, T w) noexcept {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    constexpr Vector(T (&c)[D]) noexcept {
        for (unsigned i = 0; i < D; ++i)
            this->c[i] = c[i];
    }
    // TODO: Add constructors like (Vector<T,D2>&, T, T), (Vector<T,D2>&, T (&)[D]);

    // Comparision operator
    constexpr bool operator!=(const Vector& rhs) noexcept {
        for (unsigned i = 0u; i < D; ++i)
            if (this->c[i] != rhs.c[i])
                return true;
        return false;
    }

    // Vector assignemnt operators
    template<typename T2>
    constexpr Vector& operator+=(const Vector<T2, D>& rhs) noexcept {
        for (unsigned i = 0u; i < D; ++i)
            this->c[i] += rhs.c[i];
        return *this;
    }
    template<typename T2>
    constexpr Vector& operator-=(const Vector<T2, D>& rhs) noexcept {
        for (unsigned i = 0u; i < D; ++i)
            this->c[i] -= rhs.c[i];
        return *this;
    }
    template<typename T2>

    // Scalar assignment operators
    constexpr Vector& operator*=(T2 rhs) noexcept {
        for (T& cord : this->c)
            cord *= rhs;
        return *this;
    }

    // Vector operators
    template<typename T2>
    Vector<sum_op_type_t<T, T2>, D> operator+(const Vector<T2, D>& rhs) const noexcept {
        Vector<sum_op_type_t<T, T2>, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = this->c[i] + rhs.c[i];
        return result;
    }
    template<typename T2>
    Vector<sub_op_type_t<T, T2>, D> operator-(const Vector<T2, D>& rhs) const noexcept {
        Vector<sub_op_type_t<T, T2>, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = this->c[i] - rhs.c[i];
        return result;
    }

    // Scalar operators
    template<typename T2, typename = ::std::enable_if_t<!is_vector<T2>::value>>
    Vector<sum_op_type_t<T, T2>, D> operator+(const T2& rhs) const noexcept {
        Vector<sum_op_type_t<T, T2>, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = this->c[i] + rhs;
        return result;
    }
    template<typename T2, typename = ::std::enable_if_t<!is_vector<T2>::value>>
    Vector<sub_op_type_t<T, T2>, D> operator-(T2 rhs) const noexcept {
        Vector<sub_op_type_t<T, T2>, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = this->c[i] - rhs;
        return result;
    }
    template<typename T2, typename = ::std::enable_if_t<!is_vector<T2>::value>>
    Vector<mul_op_type_t<T, T2>, D> operator*(T2 rhs) const noexcept {
        Vector<mul_op_type_t<T, T2>, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = this->c[i] * rhs;
        return result;
    }

    // Cast operator
    template<typename T2>
    explicit operator Vector<T2, D>() const noexcept {
        Vector<T2, D> result;
        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = (T2)this->c[i];
        return result;
    };

    // Special functions
    template<typename T2>
    constexpr dot_type_t<T2> dot(const Vector<T2, D>& rhs) const noexcept {
        dot_type_t<T2> sum = dot_type_t<T2>(0);
        for (unsigned i = 0u; i < D; ++i)
            sum += this->c[i] * rhs.c[i];
        return sum;
    }

    constexpr dot_type_t<T> lenSQ() const noexcept {
        dot_type_t<T> sum = 0;
        for (unsigned i = 0u; i < D; ++i)
            sum += this->c[i] * this->c[i];
        return sum;
    }
    template<typename T2=adv_math_type, typename = ::std::enable_if_t<::std::is_convertible<dot_type_t<T>, T2>::value>>
    constexpr T2 len() const noexcept { return sqrt((T2)lenSQ()); }

    template<typename T2=adv_math_type>
    Vector normalized() const noexcept {
        Vector result;
        const T2 l = len<T2>();

        constexpr auto round_cast = [](T2 x) -> T{
            return ::std::is_integral<T>::value ? T(x + T2(0.5)) : T(x);
        };

        for (unsigned i = 0u; i < D; ++i)
            result.c[i] = round_cast(this->c[i] / l);

        return result;
    }

    constexpr static dot_type_t<T> distSQ(const Vector & v1, const Vector & v2) noexcept {
        dot_type_t<T> sum = 0;
        for (unsigned i = 0; i < D; ++i) {
            const T diff = v2.c[i] - v1.c[i];
            sum += diff*diff;
        }
        return sum;
    }
    template<typename T2=adv_math_type, typename = ::std::enable_if_t<::std::is_convertible<dot_type_t<T>, T2>::value>>
    constexpr static T2 dist(const Vector & v1, const Vector & v2) noexcept { return sqrt((T2)distSQ(v1, v2)); }

    static Vector zero() noexcept {
        Vector result;
        for (T& coord : result.c)
            coord = T(0);
        return result;
    }
};

// Scalar-vector operators
template<typename T, unsigned D, typename T2=T>
inline
Vector<mul_op_type_t<T, T2>, D> operator*(T2 lhs, const Vector<T, D>& rhs) noexcept {
    Vector<mul_op_type_t<T, T2>, D> result;
    for (unsigned i = 0; i < D; ++i)
        result.c[i] = lhs * rhs.c[i];
    return result;
}

template<typename T>
using Vector2D = Vector<T, 2u>;

using Vector2Dd = Vector2D<double>;
using Vector2Di = Vector2D<int>;
using Vector2Du = Vector2D<unsigned>;

#endif // !SDLTEST_VECTOR_HPP