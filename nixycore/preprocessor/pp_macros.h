/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

//////////////////////////////////////////////////////////////////////////

/*
    Get a string of macro
*/

#define NX_PP_SOL(...)                  #__VA_ARGS__
#define NX_PP_STR_(...)                 NX_PP_SOL(__VA_ARGS__)
#define NX_PP_STR(...)                  NX_PP_STR_(__VA_ARGS__)

/*
    Connect two args together
*/

#define NX_PP_CAT(x, ...)               x##__VA_ARGS__
#define NX_PP_JOIN_(x, ...)             NX_PP_CAT(x, __VA_ARGS__)
#define NX_PP_JOIN(x, ...)              NX_PP_JOIN_(x, __VA_ARGS__)

/*
    Circumvent MSVC __VA_ARGS__ BUG
*/

#define NX_PP_VA(...)                   __VA_ARGS__ /* Try to expand __VA_ARGS__ */
#define NX_PP_PROXY(F, ...)             NX_PP_VA(F(__VA_ARGS__))
