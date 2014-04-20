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
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace interlocked
{
    struct single_thread
    {
        NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0xF);

        template <typename R, typename T>
        inline static R load(T& dest)
        {
            return reinterpret_cast<R>(dest);
        }

        template <typename T, typename U>
        inline static void store(T& dest, U val)
        {
            dest = reinterpret_cast<T>(val);
        }

        template <typename T, typename U>
        inline static T fetch_add(T& dest, U val)
        {
            T tmp = dest;
            dest += reinterpret_cast<T>(val);
            return tmp;
        }

        template <typename T, typename U>
        inline static T exchange(T& dest, U val)
        {
            T tmp = dest;
            dest = reinterpret_cast<T>(val);
            return tmp;
        }

        template <typename T, typename U>
        inline static bool compare_exchange(T& dest, U val, T comp)
        {
            if (dest == comp)
            {
                dest = reinterpret_cast<T>(val);
                return true;
            }
            else
                return false;
        }
    };

    struct multi_thread
    {
#   if defined(NX_CC_MSVC)
#       pragma warning(push)
#       pragma warning(disable: 4800)   // forcing value to bool 'true' or 'false' (performance warning)
#       pragma warning(disable: 4311)   // pointer truncation from 'type1' to 'type2'
#       pragma warning(disable: 4312)   // conversion from 'type1' to 'type2' of greater size
#       include "detail/interlocked_msvc.hxx"
#       pragma warning(pop)
#   elif defined(NX_CC_GNUC)
#       include "detail/interlocked_gnuc.hxx"
#   endif

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
            exchange(dest, val);
        }
    };
}

namespace memory_order
{
    typedef nx::type_int<0> relaxed_t;
    static const relaxed_t relaxed;

    typedef nx::type_int<1> seq_cst_t;
    static const seq_cst_t seq_cst;

    template <typename MemoryOrderT, class RelaxedModelT, class SeqCstModelT>
    struct model;

    template <class RelaxedModelT, class SeqCstModelT>
    struct model<relaxed_t, RelaxedModelT, SeqCstModelT>
    {
        typedef RelaxedModelT type_t;
    };

    template <class RelaxedModelT, class SeqCstModelT>
    struct model<seq_cst_t, RelaxedModelT, SeqCstModelT>
    {
        typedef SeqCstModelT type_t;
    };
}

template <class RelaxedModelT, class SeqCstModelT>
struct interlocked_model
{
    NX_STATIC_PROPERTY(int, SUPPORTED_MASK, SeqCstModelT::SUPPORTED_MASK);

    template <typename MemoryOrderT>
    struct model : memory_order::model<MemoryOrderT, RelaxedModelT, SeqCstModelT> {};

    typedef memory_order::seq_cst_t default_order_t;

    // load

    template <typename M, typename R, typename T>
    inline static R load(T& dest)
    {
        typedef typename model<M>::type_t type_t;
        return type_t::template load<R>(dest);
    }

    template <typename R, typename T>
    inline static R load(T& dest)
    {
        return load<default_order_t, R>(dest);
    }

    // store

    template <typename M, typename T, typename U>
    inline static void store(T& dest, U val)
    {
        typedef typename model<M>::type_t type_t;
        type_t::store(dest, val);
    }

    template <typename T, typename U>
    inline static void store(T& dest, U val)
    {
        store<default_order_t>(dest, val);
    }

    // fetch_add

    template <typename M, typename T, typename U>
    inline static T fetch_add(T& dest, U val)
    {
        typedef typename model<M>::type_t type_t;
        return type_t::fetch_add(dest, val);
    }

    template <typename T, typename U>
    inline static T fetch_add(T& dest, U val)
    {
        return fetch_add<default_order_t>(dest, val);
    }

    // exchange

    template <typename M, typename T, typename U>
    inline static T exchange(T& dest, U val)
    {
        typedef typename model<M>::type_t type_t;
        return type_t::exchange(dest, val);
    }

    template <typename T, typename U>
    inline static T exchange(T& dest, U val)
    {
        return exchange<default_order_t>(dest, val);
    }

    // compare_exchange

    template <typename M, typename T, typename U>
    inline static bool compare_exchange(T& dest, U val, T comp)
    {
        typedef typename model<M>::type_t type_t;
        return type_t::compare_exchange(dest, val, comp);
    }

    template <typename T, typename U>
    inline static bool compare_exchange(T& dest, U val, T comp)
    {
        return compare_exchange<default_order_t>(dest, val, comp);
    }
};

namespace use
{
    typedef interlocked_model<interlocked::single_thread, interlocked::single_thread> interlocked_st;
    typedef interlocked_model<interlocked::single_thread, interlocked::multi_thread > interlocked_mt;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
