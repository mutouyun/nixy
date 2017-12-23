/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"

#include "nixycore/delegate/functor.h"
#include "nixycore/finalizer/scope_guard.h"
#include "nixycore/utility/rvalue.h"

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
inline private_mem_guard::free_fr<T*> make_destructor(T* r)
{
    return private_mem_guard::free_fr<T*>(r);
}

template <typename SizeT>
inline typename nx::enable_if<nx::is_integral<SizeT>::value,
private_mem_guard::free_fr<pvoid> >::type_t make_destructor(pvoid r, SizeT s)
{
    return private_mem_guard::free_fr<pvoid>(r, s);
}

#define nx_memory_scope(...) \
    nx_guard_scope(nx::make_destructor(__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
