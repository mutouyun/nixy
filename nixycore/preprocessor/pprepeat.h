/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

// preprocessor macros
#include "nixycore/preprocessor/ppmacros.h"

//////////////////////////////////////////////////////////////////////////

#define NX_PP_REPEAT_0(f1, f2, ...)

#define NX_PP_REPEAT_1(f1, f2, ...)         NX_PP_VA(f1(1, __VA_ARGS__))
#define NX_PP_REPEAT_2(f1, f2, ...)         NX_PP_REPEAT_1(f1, f2, __VA_ARGS__) NX_PP_VA(f2(2, __VA_ARGS__))
#define NX_PP_REPEAT_3(f1, f2, ...)         NX_PP_REPEAT_2(f1, f2, __VA_ARGS__) NX_PP_VA(f2(3, __VA_ARGS__))
#define NX_PP_REPEAT_4(f1, f2, ...)         NX_PP_REPEAT_3(f1, f2, __VA_ARGS__) NX_PP_VA(f2(4, __VA_ARGS__))
#define NX_PP_REPEAT_5(f1, f2, ...)         NX_PP_REPEAT_4(f1, f2, __VA_ARGS__) NX_PP_VA(f2(5, __VA_ARGS__))
#define NX_PP_REPEAT_6(f1, f2, ...)         NX_PP_REPEAT_5(f1, f2, __VA_ARGS__) NX_PP_VA(f2(6, __VA_ARGS__))
#define NX_PP_REPEAT_7(f1, f2, ...)         NX_PP_REPEAT_6(f1, f2, __VA_ARGS__) NX_PP_VA(f2(7, __VA_ARGS__))
#define NX_PP_REPEAT_8(f1, f2, ...)         NX_PP_REPEAT_7(f1, f2, __VA_ARGS__) NX_PP_VA(f2(8, __VA_ARGS__))
#define NX_PP_REPEAT_9(f1, f2, ...)         NX_PP_REPEAT_8(f1, f2, __VA_ARGS__) NX_PP_VA(f2(9, __VA_ARGS__))
#define NX_PP_REPEAT_10(f1, f2, ...)        NX_PP_REPEAT_9(f1, f2, __VA_ARGS__) NX_PP_VA(f2(10, __VA_ARGS__))

#define NX_PP_REPEAT_11(f1, f2, ...)        NX_PP_REPEAT_10(f1, f2, __VA_ARGS__) NX_PP_VA(f2(11, __VA_ARGS__))
#define NX_PP_REPEAT_12(f1, f2, ...)        NX_PP_REPEAT_11(f1, f2, __VA_ARGS__) NX_PP_VA(f2(12, __VA_ARGS__))
#define NX_PP_REPEAT_13(f1, f2, ...)        NX_PP_REPEAT_12(f1, f2, __VA_ARGS__) NX_PP_VA(f2(13, __VA_ARGS__))
#define NX_PP_REPEAT_14(f1, f2, ...)        NX_PP_REPEAT_13(f1, f2, __VA_ARGS__) NX_PP_VA(f2(14, __VA_ARGS__))
#define NX_PP_REPEAT_15(f1, f2, ...)        NX_PP_REPEAT_14(f1, f2, __VA_ARGS__) NX_PP_VA(f2(15, __VA_ARGS__))
#define NX_PP_REPEAT_16(f1, f2, ...)        NX_PP_REPEAT_15(f1, f2, __VA_ARGS__) NX_PP_VA(f2(16, __VA_ARGS__))
#define NX_PP_REPEAT_17(f1, f2, ...)        NX_PP_REPEAT_16(f1, f2, __VA_ARGS__) NX_PP_VA(f2(17, __VA_ARGS__))
#define NX_PP_REPEAT_18(f1, f2, ...)        NX_PP_REPEAT_17(f1, f2, __VA_ARGS__) NX_PP_VA(f2(18, __VA_ARGS__))
#define NX_PP_REPEAT_19(f1, f2, ...)        NX_PP_REPEAT_18(f1, f2, __VA_ARGS__) NX_PP_VA(f2(19, __VA_ARGS__))
#define NX_PP_REPEAT_20(f1, f2, ...)        NX_PP_REPEAT_19(f1, f2, __VA_ARGS__) NX_PP_VA(f2(20, __VA_ARGS__))

/*
    NX_PP_REPEAT(5, f, data)
    -->
    f(1, data) f(2, data) f(3, data) f(4, data) f(5, data)
*/

#define NX_PP_REPEAT_P(F, ...)              NX_PP_VA(F(__VA_ARGS__))

#define NX_PP_REPEATEX(n, f1, f2, ...)      NX_PP_REPEAT_P(NX_PP_JOIN(NX_PP_REPEAT_, n), f1, f2, __VA_ARGS__)
#define NX_PP_REPEATEX_MAX(f1, f2, ...)     NX_PP_REPEATEX(NX_PP_MAX, f1, f2, __VA_ARGS__)

#define NX_PP_REPEAT(n, f, ...)             NX_PP_REPEATEX(n, f, f, __VA_ARGS__)
#define NX_PP_REPEAT_MAX(f, ...)            NX_PP_REPEATEX_MAX(f, f, __VA_ARGS__)
