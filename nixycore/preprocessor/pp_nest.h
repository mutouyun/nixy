/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

// NX_PP_COUNT
#include "nixycore/preprocessor/pp_count.h"

//////////////////////////////////////////////////////////////////////////

#define NX_PP_NEST_0(f, l, n, ...)

#define NX_PP_NEST_1(f, l, n, ...)              NX_PP_VA(l(NX_PP_VA(__VA_ARGS__)))
#define NX_PP_NEST_2(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_1(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_3(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_2(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_4(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_3(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_5(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_4(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_6(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_5(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_7(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_6(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_8(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_7(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_9(f, l, n, ...)              NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_8(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_10(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_9(f, l, n, n(__VA_ARGS__))))

#define NX_PP_NEST_11(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_10(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_12(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_11(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_13(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_12(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_14(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_13(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_15(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_14(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_16(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_15(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_17(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_16(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_18(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_17(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_19(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_18(f, l, n, n(__VA_ARGS__))))
#define NX_PP_NEST_20(f, l, n, ...)             NX_PP_VA(f(NX_PP_VA(__VA_ARGS__), NX_PP_NEST_19(f, l, n, n(__VA_ARGS__))))

/*
    NX_PP_NEST(3, func, last, nest, xx)
    -->
    func(xx, func(nest(xx), last(nest(nest(xx)))))
*/

#define NX_PP_NEST_P(F, ...)                    NX_PP_VA(F(__VA_ARGS__))
#define NX_PP_NEST(n, func, last, nest, ...)    NX_PP_NEST_P(NX_PP_JOIN(NX_PP_NEST_, n), func, last, nest, __VA_ARGS__)
#define NX_PP_NEST_MAX(func, last, nest, ...)   NX_PP_NEST(NX_PP_MAX, func, last, nest, __VA_ARGS__)
