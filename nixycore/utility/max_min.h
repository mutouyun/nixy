/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Get the maximum of variables
*/

#define NX_MAX(x, y) (((x) > (y)) ? (x) : (y))

template <typename T>
inline const T& maxof(const T& x, const T& y)
{
    return NX_MAX(x, y);
}

#ifndef nx_max
#define nx_max(...) NX_PP_VA(NX_PP_ORDER(nx::maxof, __VA_ARGS__))
#endif

/*
    Get the minimum of variables
*/

#define NX_MIN(x, y) (((x) < (y)) ? (x) : (y))

template <typename T>
inline const T& minof(const T& x, const T& y)
{
    return NX_MIN(x, y);
}

#ifndef nx_min
#define nx_min(...) NX_PP_VA(NX_PP_ORDER(nx::minof, __VA_ARGS__))
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
