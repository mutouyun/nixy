/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

// preprocessor macros
#include "nixycore/preprocessor/pp_macros.h"

//////////////////////////////////////////////////////////////////////////

#define NX_PP_MULT_0(f)

#define NX_PP_MULT_1(f)         f(1)
#define NX_PP_MULT_2(f)         NX_PP_MULT_1(f) f(2)
#define NX_PP_MULT_3(f)         NX_PP_MULT_2(f) f(3)
#define NX_PP_MULT_4(f)         NX_PP_MULT_3(f) f(4)
#define NX_PP_MULT_5(f)         NX_PP_MULT_4(f) f(5)
#define NX_PP_MULT_6(f)         NX_PP_MULT_5(f) f(6)
#define NX_PP_MULT_7(f)         NX_PP_MULT_6(f) f(7)
#define NX_PP_MULT_8(f)         NX_PP_MULT_7(f) f(8)
#define NX_PP_MULT_9(f)         NX_PP_MULT_8(f) f(9)
#define NX_PP_MULT_10(f)        NX_PP_MULT_9(f) f(10)

#define NX_PP_MULT_11(f)        NX_PP_MULT_10(f) f(11)
#define NX_PP_MULT_12(f)        NX_PP_MULT_11(f) f(12)
#define NX_PP_MULT_13(f)        NX_PP_MULT_12(f) f(13)
#define NX_PP_MULT_14(f)        NX_PP_MULT_13(f) f(14)
#define NX_PP_MULT_15(f)        NX_PP_MULT_14(f) f(15)
#define NX_PP_MULT_16(f)        NX_PP_MULT_15(f) f(16)
#define NX_PP_MULT_17(f)        NX_PP_MULT_16(f) f(17)
#define NX_PP_MULT_18(f)        NX_PP_MULT_17(f) f(18)
#define NX_PP_MULT_19(f)        NX_PP_MULT_18(f) f(19)
#define NX_PP_MULT_20(f)        NX_PP_MULT_19(f) f(20)

/*
    NX_PP_MULT(10, f)
    -->
    f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(10)
*/

#define NX_PP_MULT_P(F, ...)    NX_PP_VA(F(__VA_ARGS__))
#define NX_PP_MULT(n, f)        NX_PP_MULT_P(NX_PP_JOIN(NX_PP_MULT_, n), f)
#define NX_PP_MULT_MAX(f)       NX_PP_MULT(NX_PP_MAX, f)
