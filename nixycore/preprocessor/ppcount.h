/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

// preprocessor macros
#include "preprocessor/ppmacros.h"

//////////////////////////////////////////////////////////////////////////

#define NX_PP_MAX       20

#define NX_PP_FILTER_( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, \
                       _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                       _N, ...) _N

#define NX_PP_NUMBER_() 20,  19,  18,  17,  16,  15,  14,  13,  12,  11, \
                        10,   9,   8,   7,   6,   5,   4,   3,   2,   1

/*
    Get count of args from __VA_ARGS__

    NX_PP_COUNT(a, b, c, d)
    -->
    4
*/

#define NX_PP_HELPER(...)   NX_PP_VA(NX_PP_FILTER_(__VA_ARGS__))
#define NX_PP_COUNT(...)    NX_PP_HELPER(__VA_ARGS__, NX_PP_NUMBER_())
