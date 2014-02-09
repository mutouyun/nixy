/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_OS_WIN)
#   include "nixycore/al/thread/threadops_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "nixycore/al/thread/threadops_linux.hxx"
#endif

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC) && defined(NX_PC_X86)

extern "C" void _mm_pause(void);
#pragma intrinsic(_mm_pause)
#define NX_THREADOPS_PAUSE() _mm_pause()

#elif defined(NX_CC_GNUC) && defined(NX_PC_X86)

#define NX_THREADOPS_PAUSE() __asm__ __volatile__ ("pause")

#endif

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
