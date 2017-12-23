/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

//////////////////////////////////////////////////////////////////////////

#include "interlocked_def_msvc.hxx"

#if defined(NX_OS_WINCE)
#   define NX_CC_BARRIER()
#else /*NX_OS_WINCE*/
#   include <intrin.h>
#   pragma intrinsic(_ReadWriteBarrier)
#   define NX_CC_BARRIER() _ReadWriteBarrier()
#endif/*NX_OS_WINCE*/

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

inline static void cc_barrier(void)
{
    NX_CC_BARRIER();
}

inline static void mm_barrier(void)
{
    NX_CC_BARRIER();
#if defined(NX_OS_WINCE)
    long tmp;
    NX_INTERLOCKED_EXCHANGE(tmp, 0);
#else
    MemoryBarrier();
#endif
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
