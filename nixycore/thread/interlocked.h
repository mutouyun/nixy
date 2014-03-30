/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/barrier.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
#include "nixycore/general/disable_warnings.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace use
{
    struct interlocked_st
    {
        NX_STATIC_PROPERTY(int, supported_mask, 0xF);

        template <typename R, typename T>
        inline static R load(T& dest)
        {
            return (R)dest;
        }

        template <typename T, typename U>
        inline static void store(T& dest, U val)
        {
            dest = (T)val;
        }

        template <typename T, typename U>
        inline static T fetch_add(T& dest, U val)
        {
            T tmp = dest;
            dest += (T)val;
            return tmp;
        }

        template <typename T, typename U>
        inline static T exchange(T& dest, U val)
        {
            T tmp = dest;
            dest = (T)val;
            return tmp;
        }

        template <typename T, typename U>
        inline static bool compare_exchange(T& dest, U val, T comp)
        {
            if (dest == comp)
            {
                dest = (T)val;
                return true;
            }
            else
                return false;
        }
    };
}

namespace private_interlocked
{
    template <typename M>
    struct base
    {
        template <typename R, typename T>
        inline static R load(T& dest)
        {
            R r = reinterpret_cast<const volatile R &>(const_cast<const volatile T &>(dest));
#       if defined(NX_PC_X86)
            /* On x86 and x86_64 there is no need for a hardware barrier */
            thread_ops::cc_barrier();
#       else
            thread_ops::mm_barrier();
#       endif
            return r;
        }

        template <typename T, typename U>
        inline static void store(T& dest, U val)
        {
            M::exchange(dest, val);
        }
    };
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   include "detail/interlocked_msvc.hxx"
#elif defined(NX_CC_GNUC)
#   include "detail/interlocked_gnuc.hxx"
#endif

#include "nixycore/general/disable_warnings.h"
//////////////////////////////////////////////////////////////////////////