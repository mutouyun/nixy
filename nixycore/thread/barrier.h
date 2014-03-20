/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   include "detail/barrier_msvc.hxx"
#elif defined(NX_CC_GNUC)
#   include "detail/barrier_gnuc.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
