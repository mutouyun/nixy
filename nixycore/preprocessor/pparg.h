/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

// preprocessor macros
#include "nixycore/preprocessor/ppmacros.h"

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_B2(a, b, c, d, e)
    -->
    c, d, e
*/

#define NX_PP_B_H(a, ...)   __VA_ARGS__

#define NX_PP_B0(...)       NX_PP_VA(__VA_ARGS__)
#define NX_PP_B1(...)       NX_PP_VA(NX_PP_B_H(__VA_ARGS__))
#define NX_PP_B2(...)       NX_PP_B1(NX_PP_B1(__VA_ARGS__))
#define NX_PP_B3(...)       NX_PP_B1(NX_PP_B2(__VA_ARGS__))
#define NX_PP_B4(...)       NX_PP_B1(NX_PP_B3(__VA_ARGS__))
#define NX_PP_B5(...)       NX_PP_B1(NX_PP_B4(__VA_ARGS__))
#define NX_PP_B6(...)       NX_PP_B1(NX_PP_B5(__VA_ARGS__))
#define NX_PP_B7(...)       NX_PP_B1(NX_PP_B6(__VA_ARGS__))
#define NX_PP_B8(...)       NX_PP_B1(NX_PP_B7(__VA_ARGS__))
#define NX_PP_B9(...)       NX_PP_B1(NX_PP_B8(__VA_ARGS__))

#define NX_PP_B10(...)      NX_PP_B1(NX_PP_B9(__VA_ARGS__))
#define NX_PP_B11(...)      NX_PP_B1(NX_PP_B10(__VA_ARGS__))
#define NX_PP_B12(...)      NX_PP_B1(NX_PP_B11(__VA_ARGS__))
#define NX_PP_B13(...)      NX_PP_B1(NX_PP_B12(__VA_ARGS__))
#define NX_PP_B14(...)      NX_PP_B1(NX_PP_B13(__VA_ARGS__))
#define NX_PP_B15(...)      NX_PP_B1(NX_PP_B14(__VA_ARGS__))
#define NX_PP_B16(...)      NX_PP_B1(NX_PP_B15(__VA_ARGS__))
#define NX_PP_B17(...)      NX_PP_B1(NX_PP_B16(__VA_ARGS__))
#define NX_PP_B18(...)      NX_PP_B1(NX_PP_B17(__VA_ARGS__))
#define NX_PP_B19(...)      NX_PP_B1(NX_PP_B18(__VA_ARGS__))

#define NX_PP_B20(...)      NX_PP_B1(NX_PP_B19(__VA_ARGS__))

#define NX_PP_B_P(F, ...)   NX_PP_VA(F(__VA_ARGS__))
#define NX_PP_B(n, ...)     NX_PP_B_P(NX_PP_JOIN(NX_PP_B, n), __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_A3(a, b, c, d, e)
    -->
    c
*/

#define NX_PP_A_H(a, ...)   a

#define NX_PP_A0(...)
#define NX_PP_A1(...)       NX_PP_VA(NX_PP_A_H(__VA_ARGS__))
#define NX_PP_A2(...)       NX_PP_A1(NX_PP_B1(__VA_ARGS__))
#define NX_PP_A3(...)       NX_PP_A1(NX_PP_B2(__VA_ARGS__))
#define NX_PP_A4(...)       NX_PP_A1(NX_PP_B3(__VA_ARGS__))
#define NX_PP_A5(...)       NX_PP_A1(NX_PP_B4(__VA_ARGS__))
#define NX_PP_A6(...)       NX_PP_A1(NX_PP_B5(__VA_ARGS__))
#define NX_PP_A7(...)       NX_PP_A1(NX_PP_B6(__VA_ARGS__))
#define NX_PP_A8(...)       NX_PP_A1(NX_PP_B7(__VA_ARGS__))
#define NX_PP_A9(...)       NX_PP_A1(NX_PP_B8(__VA_ARGS__))

#define NX_PP_A10(...)      NX_PP_A1(NX_PP_B9(__VA_ARGS__))
#define NX_PP_A11(...)      NX_PP_A1(NX_PP_B10(__VA_ARGS__))
#define NX_PP_A12(...)      NX_PP_A1(NX_PP_B11(__VA_ARGS__))
#define NX_PP_A13(...)      NX_PP_A1(NX_PP_B12(__VA_ARGS__))
#define NX_PP_A14(...)      NX_PP_A1(NX_PP_B13(__VA_ARGS__))
#define NX_PP_A15(...)      NX_PP_A1(NX_PP_B14(__VA_ARGS__))
#define NX_PP_A16(...)      NX_PP_A1(NX_PP_B15(__VA_ARGS__))
#define NX_PP_A17(...)      NX_PP_A1(NX_PP_B16(__VA_ARGS__))
#define NX_PP_A18(...)      NX_PP_A1(NX_PP_B17(__VA_ARGS__))
#define NX_PP_A19(...)      NX_PP_A1(NX_PP_B18(__VA_ARGS__))

#define NX_PP_A20(...)      NX_PP_A1(NX_PP_B19(__VA_ARGS__))

#define NX_PP_A_P(F, ...)   NX_PP_VA(F(__VA_ARGS__))
#define NX_PP_A(n, ...)     NX_PP_A_P(NX_PP_JOIN(NX_PP_A, n), __VA_ARGS__)
