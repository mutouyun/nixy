/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_GNUC)
#   define NX_TYPEOF_(...) __typeof(__VA_ARGS__)
#elif defined(NX_CC_MSVC)
#   include "detail/typeof_msvc.hxx"
#else
#   define NX_TYPEOF_(...) nx::null_t
#endif

#ifndef nx_typeof
#define nx_typeof(...) NX_TYPEOF_(__VA_ARGS__)
#endif

#ifndef nx_auto
#ifdef NX_SP_CXX11_AUTO
#define nx_auto(name, ...) auto name((__VA_ARGS__))
#else/*NX_SP_CXX11_AUTO*/
#define nx_auto(name, ...) nx_typeof(__VA_ARGS__) name((__VA_ARGS__))
#endif/*NX_SP_CXX11_AUTO*/
#endif

//////////////////////////////////////////////////////////////////////////
