/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_ATOMIC
#include <atomic> // std::atomic ...

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

inline static void cc_barrier(void)
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

inline static void mm_barrier(void)
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
#else /*NX_SP_CXX11_ATOMIC*/

#if defined(NX_CC_MSVC)
#   include "detail/barrier_msvc.hxx"
#elif defined(NX_CC_GNUC)
#   include "detail/barrier_gnuc.hxx"
#endif

#endif/*NX_SP_CXX11_ATOMIC*/
