/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_THREAD
#   include "detail/thread_ops_std.hxx"
#elif defined(NX_OS_WIN)
#   include "detail/thread_ops_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "detail/thread_ops_linux.hxx"
#endif

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC) && defined(NX_PC_X86)

extern "C" void _mm_pause(void);
#pragma intrinsic(_mm_pause)
#define NX_THREADOPS_PAUSE() _mm_pause()

#elif defined(NX_CC_GNUC) && defined(NX_PC_X86)

#define NX_THREADOPS_PAUSE() __asm__ __volatile__ ("pause")

#endif/*defined(NX_CC_) && defined(NX_PC_)*/

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

inline void pause(unsigned k = 4)
{
    if (k < 4)  {}
#ifdef NX_THREADOPS_PAUSE
    else
    if (k < 16) { NX_THREADOPS_PAUSE(); }
#endif/*NX_THREADOPS_PAUSE*/
    else
    if (k < 32) { yield(); }
    else
    {
        sleep(1);
    }
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
