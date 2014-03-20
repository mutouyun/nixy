/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

#if defined(NX_OS_LINUX)
#include <time.h>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

inline ulong tickcount(void)
{
#if defined(NX_OS_WIN)
    return static_cast<ulong>(GetTickCount());
#elif defined(NX_OS_LINUX)
    /* need -lrt */
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (tv.tv_sec * 1000) + (tv.tv_nsec / 1000000);
#endif
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
