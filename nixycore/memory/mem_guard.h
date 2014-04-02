/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/delegate/functor.h"
#include "nixycore/finalizer/scope_guard.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    memory scope guard
*/

namespace private_mem_guard
{
    template <typename T>
    struct free_fr
    {
        T p_;
        free_fr(T p) : p_(p) {}
        void operator()(void) { nx::free(p_); }
    };

    template <>
    struct free_fr<pvoid>
    {
        pvoid p_;
        size_t s_;
        free_fr(pvoid p, size_t s) : p_(p), s_(s) {}
        void operator()(void) { nx::free(p_, s_); }
    };
}

template <typename T>
inline typename enable_if<is_pointer<T>::value &&
                         !is_function<T>::value,
functor<void()> >::type_t make_destructor(T r)
{
    return private_mem_guard::free_fr<T>(r);
}

inline functor<void()> make_destructor(pvoid r, size_t s)
{
    return private_mem_guard::free_fr<pvoid>(r, s);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
