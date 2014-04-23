/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/barrier.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

#ifdef NX_SP_CXX11_ATOMIC
#include <atomic> // std::atomic ...
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace memory_order
{
#ifdef NX_SP_CXX11_ATOMIC
    typedef nx::type_int<std::memory_order_relaxed> relaxed_t;
    typedef nx::type_int<std::memory_order_acquire> acquire_t;
    typedef nx::type_int<std::memory_order_release> release_t;
    typedef nx::type_int<std::memory_order_acq_rel> acq_rel_t;
    typedef nx::type_int<std::memory_order_seq_cst> seq_cst_t;
#else
    typedef nx::type_int<1> relaxed_t;
    typedef nx::type_int<2> acquire_t;
    typedef nx::type_int<3> release_t;
    typedef nx::type_int<4> acq_rel_t;
    typedef nx::type_int<5> seq_cst_t;
#endif
    static const relaxed_t relaxed = relaxed_t();
    static const acquire_t acquire = acquire_t();
    static const release_t release = release_t();
    static const acq_rel_t acq_rel = acq_rel_t();
    static const seq_cst_t seq_cst = seq_cst_t();
}

namespace interlocked
{
    struct order_freedom
    {
        NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0xF);

        // memory barrier

        template <typename M>
        inline static void signal_fence(M)
        {
            // do nothing
        }

        template <typename M>
        inline static void thread_fence(M)
        {
            // do nothing
        }

        // relaxed, seq_cst, ...

        template <typename T, typename M>
        inline static T load(const T& dest, M)
        {
            return dest;
        }

        template <typename T, typename U, typename M>
        inline static void store(T& dest, U val, M)
        {
            dest = (T)(val);
        }

        template <typename T, typename U, typename M>
        inline static T fetch_add(T& dest, U val, M)
        {
            T tmp = dest;
            dest += (T)(val);
            return tmp;
        }

        template <typename T, typename U, typename M>
        inline static T exchange(T& dest, U val, M)
        {
            T tmp = dest;
            dest = (T)(val);
            return tmp;
        }

        template <typename T, typename U, typename M>
        inline static bool compare_exchange(T& dest, U val, T comp, M)
        {
            if (dest == comp)
            {
                dest = (T)(val);
                return true;
            }
            else
                return false;
        }
    };

    struct order_default
    {
#   ifdef NX_SP_CXX11_ATOMIC

#   ifdef NX_PC_X86_64
        NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0xF);
#   else
        NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0x7);
#   endif

        // memory barrier

        template <typename M>
        inline static void signal_fence(M)
        {
            std::atomic_signal_fence(static_cast<std::memory_order>(M::value));
        }

        template <typename M>
        inline static void thread_fence(M)
        {
            std::atomic_thread_fence(static_cast<std::memory_order>(M::value));
        }

        // relaxed, seq_cst, ...

#   define NX_ATOMIC_TYPE_(T) std::atomic<T>

        template <typename T, typename M>
        inline static T load(const NX_ATOMIC_TYPE_(T)& dest, M)
        {
            return std::atomic_load_explicit(&dest, static_cast<std::memory_order>(M::value));
        }

        template <typename T, typename U, typename M>
        inline static void store(NX_ATOMIC_TYPE_(T)& dest, U val, M)
        {
            std::atomic_store_explicit(&dest, (T)(val)
                                            , static_cast<std::memory_order>(M::value));
        }

        template <typename T, typename U, typename M>
        inline static T fetch_add(NX_ATOMIC_TYPE_(T)& dest, U val, M)
        {
            return std::atomic_fetch_add_explicit(&dest, (T)(val)
                                                       , static_cast<std::memory_order>(M::value));
        }

        template <typename T, typename U, typename M>
        inline static T exchange(NX_ATOMIC_TYPE_(T)& dest, U val, M)
        {
            return std::atomic_exchange_explicit(&dest, (T)(val)
                                                      , static_cast<std::memory_order>(M::value));
        }

        template <typename T, typename U, typename M>
        inline static bool compare_exchange(NX_ATOMIC_TYPE_(T)& dest, U val, T comp, M)
        {
            return std::atomic_compare_exchange_strong_explicit(&dest, &comp, (T)(val)
                                                                            , static_cast<std::memory_order>(M::value)
                                                                            , static_cast<std::memory_order>(M::value));
        }

#   else /*NX_SP_CXX11_ATOMIC*/

        // memory barrier

        inline static void signal_fence(memory_order::relaxed_t m)
        {
            order_freedom::signal_fence(m);
        }

        inline static void thread_fence(memory_order::relaxed_t m)
        {
            order_freedom::thread_fence(m);
        }

        inline static void thread_fence(memory_order::seq_cst_t)
        {
            thread_ops::mm_barrier();
        }

        template <typename M>
        inline static void signal_fence(M) // acquire, release, acq_rel, seq_cst
        {
            thread_ops::cc_barrier();
        }

        template <typename M>
        inline static void thread_fence(M) // acquire, release, acq_rel
        {
            thread_ops::cc_barrier();
        }

    private:

        // fence_before

        inline static void fence_before(memory_order::acquire_t)
        {
            // Do nothing
        }

        template <typename M>
        inline static void fence_before(M m) // relaxed, release, acq_rel, seq_cst
        {
#       if defined(NX_PC_X86)
            signal_fence(m);
#       else
            thread_fence(m);
#       endif
        }

        // fence_after

        inline static void fence_after(memory_order::release_t)
        {
            // Do nothing
        }

        template <typename M>
        inline static void fence_after(M m) // relaxed, acquire, acq_rel, seq_cst
        {
#       if defined(NX_PC_X86)
            signal_fence(m);
#       else
            thread_fence(m);
#       endif
        }

        template <typename M>
        class fence_guard
        {
            M& order_;

        public:
            inline fence_guard (M& m)
                : order_(m)           { fence_before(order_); }
            inline ~fence_guard(void) { fence_after (order_); }
        };

    public:

        // relaxed, seq_cst, ...

#   define NX_ATOMIC_TYPE_(T) T
#   define NX_FENCE_GUARD_(order) \
        fence_guard<nx_typeof(order)> NX_UNUSED guard__(m)

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

        template <typename T, typename M>
        inline static T load(const NX_ATOMIC_TYPE_(T)& dest, M m)
        {
            T r = const_cast<const volatile T &>(dest);
            fence_after(m);
            return r;
        }

        template <typename T, typename U>
        inline static void store(NX_ATOMIC_TYPE_(T)& dest, U val, memory_order::seq_cst_t m)
        {
            exchange(dest, val, m);
        }

        template <typename T, typename U, typename M>
        inline static void store(NX_ATOMIC_TYPE_(T)& dest, U val, M m)
        {
#       if defined(NX_PC_X86)
            fence_before(m);
            const_cast<volatile T &>(dest) = (T)(val);
#       else
            exchange(dest, val, m);
#       endif
        }

#   undef NX_FENCE_GUARD_
#   endif/*NX_SP_CXX11_ATOMIC*/
    };

    template <class OrderT>
    struct model : OrderT
    {
        typedef memory_order::seq_cst_t default_order_t;

        // memory barrier

        using OrderT::signal_fence;

        inline static void signal_fence(void)
        {
            OrderT::signal_fence(default_order_t());
        }

        using OrderT::thread_fence;

        inline static void thread_fence(void)
        {
            OrderT::thread_fence(default_order_t());
        }

        // load

        using OrderT::load;

        template <typename T>
        inline static T load(const NX_ATOMIC_TYPE_(T)& dest)
        {
            return load(dest, default_order_t());
        }

        // store

        using OrderT::store;

        template <typename T, typename U>
        inline static void store(NX_ATOMIC_TYPE_(T)& dest, U val)
        {
            store(dest, val, default_order_t());
        }

        // fetch_add

        using OrderT::fetch_add;

        template <typename T, typename U>
        inline static T fetch_add(NX_ATOMIC_TYPE_(T)& dest, U val)
        {
            return fetch_add(dest, val, default_order_t());
        }

        // exchange

        using OrderT::exchange;

        template <typename T, typename U>
        inline static T exchange(NX_ATOMIC_TYPE_(T)& dest, U val)
        {
            return exchange(dest, val, default_order_t());
        }

        // compare_exchange

        using OrderT::compare_exchange;

        template <typename T, typename U>
        inline static bool compare_exchange(NX_ATOMIC_TYPE_(T)& dest, U val, T comp)
        {
            return compare_exchange(dest, val, comp, default_order_t());
        }
    };

#undef NX_ATOMIC_TYPE_
}

namespace use
{
    typedef interlocked::model<interlocked::order_freedom> interlocked_st;
    typedef interlocked::model<interlocked::order_default> interlocked_mt;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
