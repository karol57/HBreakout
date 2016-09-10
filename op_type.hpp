//
// Created by Karol on 10.09.2016.
//

#ifndef SDLTEST_OP_TYPE_HPP
#define SDLTEST_OP_TYPE_HPP

template <typename A, typename B>
struct sum_op_type { A a; B b; using type = decltype(a + b); };
template <typename A, typename B>
struct sub_op_type { A a; B b; using type = decltype(a - b); };
template <typename A, typename B>
struct mul_op_type { A a; B b; using type = decltype(a * b); };
template <typename A, typename B>
struct div_op_type { A a; B b; using type = decltype(a / b); };

template<typename A, typename B>
using sum_op_type_t = typename sum_op_type<A, B>::type;
template<typename A, typename B>
using sub_op_type_t = typename sub_op_type<A, B>::type;
template<typename A, typename B>
using mul_op_type_t = typename div_op_type<A, B>::type;
template<typename A, typename B>
using div_op_type_t = typename mul_op_type<A, B>::type;

#endif // !SDLTEST_OP_TYPE_HPP