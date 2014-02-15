/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#include "nixycore/al/typemanip/typeof.hxx"

#ifndef NX_TYPEOF_
#define NX_TYPEOF_(...) nx::null_t
#endif

#ifndef nx_typeof
#define nx_typeof(...) NX_TYPEOF_(__VA_ARGS__)
#endif

template <typename T> struct rm_rvalue             { typedef T type_t; };
template <typename T> struct rm_rvalue<rvalue<T> > { typedef T type_t; };

#ifndef nx_auto
#define nx_auto(name, ...) nx::rm_rvalue<nx_typeof(__VA_ARGS__)>::type_t name((__VA_ARGS__))
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
