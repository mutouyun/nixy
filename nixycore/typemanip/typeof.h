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
NX_BEG
//////////////////////////////////////////////////////////////////////////

#include "nixycore/al/typemanip/typeof.hxx"

#ifndef nx_typeof
#define nx_typeof(...) nx::null_t
#endif

#ifndef nx_auto
#define nx_auto(name, ...) nx_typeof(__VA_ARGS__) name((__VA_ARGS__))
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
