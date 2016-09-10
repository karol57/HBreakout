//
// Created by Karol on 08.09.2016.
//

#ifndef SDLTEST_VECCOORDS_HPP
#define SDLTEST_VECCOORDS_HPP

template<typename T, unsigned D>
struct VecCoords {
    static_assert(D > 0, "Vector should have at least 1 dimension");
    union {
        struct { T x, y, z, w; };
        T c[D];
    };
};

template<typename T> struct VecCoords<T, 1> { union { T x; T c[1]; }; };
template<typename T> struct VecCoords<T, 2> { union { struct { T x, y; }; T c[2]; }; };
template<typename T> struct VecCoords<T, 3> { union { struct { T x, y, z; }; T c[3]; }; };

#endif // !SDLTEST_VECCOORDS_HPP