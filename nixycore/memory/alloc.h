/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/mem_alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifndef NX_DEFAULT_ALLOC
#define NX_DEFAULT_ALLOC nx::use::alloc_pool
#endif

/*
    construct alloc
*/

inline pvoid alloc(size_t size)
{
    return NX_DEFAULT_ALLOC::alloc(size);
}

namespace private_alloc
{
    template <typename AllocT, typename T>
    struct detail
    {
        static T* alloc(void)
        {
            return nx_construct(AllocT::alloc(sizeof(T)), T);
        }

#   define NX_ALLOC_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        static T* alloc(NX_PP_TYPE_2(n, P, par)) \
        { \
            return nx_construct(AllocT::alloc(sizeof(T)), T, NX_PP_TYPE_1(n, par)); \
        }
        NX_PP_MULT_MAX(NX_ALLOC_)
#   undef NX_ALLOC_
    };

    /* Make array to array pointer */
    template <typename AllocT, typename T_, size_t N>
    struct detail<AllocT, T_[N]>
    {
        typedef T_ T[N];
        static T* alloc(void)
        {
            T* p = (T*)AllocT::alloc(sizeof(T));
            nx_construct_arr(*p, T_, N);
            return p;
        }

#   define NX_ALLOC_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        static T* alloc(NX_PP_TYPE_2(n, P, par)) \
        { \
            T* p = (T*)AllocT::alloc(sizeof(T)); \
            nx_construct_arr(*p, T_, N, NX_PP_TYPE_1(n, par)); \
            return p; \
        }
        NX_PP_MULT_MAX(NX_ALLOC_)
#   undef NX_ALLOC_
    };

    template <typename AllocT>
    struct detail<AllocT, void>
    {
        static void* alloc(size_t size)
        {
            return AllocT::alloc(size);
        }
    };
}

template <typename T>
inline T* alloc(void)
{
    return private_alloc::detail<NX_DEFAULT_ALLOC, T>::alloc();
}

template <typename AllocT, typename T>
inline T* alloc(void)
{
    return private_alloc::detail<AllocT, T>::alloc();
}

#define NX_ALLOC_(n) \
template <typename T, NX_PP_TYPE_1(n, typename P)> \
inline T* alloc(NX_PP_TYPE_2(n, P, par)) \
{ \
    return private_alloc::detail<NX_DEFAULT_ALLOC, T>::alloc(NX_PP_TYPE_1(n, par)); \
} \
template <typename AllocT, typename T, NX_PP_TYPE_1(n, typename P)> \
inline T* alloc(NX_PP_TYPE_2(n, P, par)) \
{ \
    return private_alloc::detail<AllocT, T>::alloc(NX_PP_TYPE_1(n, par)); \
}
NX_PP_MULT_MAX(NX_ALLOC_)
#undef NX_ALLOC_

/*
    destruct free
*/

inline void free(pvoid p)
{
    NX_DEFAULT_ALLOC::free(p);
}

template <typename AllocT>
inline void free(pvoid p)
{
    AllocT::free(p);
}

template <typename T>
inline void free(T* p)
{
    if (!p) return;
    nx_destruct(p, T);
    NX_DEFAULT_ALLOC::free(p);
}

template <typename AllocT, typename T>
inline void free(T* p)
{
    if (!p) return;
    nx_destruct(p, T);
    AllocT::free(p);
}

template <typename T, size_t N>
inline void free(T(* p)[N])
{
    if (!p) return;
    nx_destruct_arr(*p, T, N);
    NX_DEFAULT_ALLOC::free(p);
}

template <typename AllocT, typename T, size_t N>
inline void free(T(* p)[N])
{
    if (!p) return;
    nx_destruct_arr(*p, T, N);
    AllocT::free(p);
}

/*
    realloc
*/

inline pvoid realloc(pvoid p, size_t size)
{
    return NX_DEFAULT_ALLOC::realloc(p, size);
}

template <typename AllocT>
inline pvoid realloc(pvoid p, size_t size)
{
    return AllocT::realloc(p, size);
}

template <typename T>
inline pvoid realloc(T* p, size_t size)
{
    if (p) nx_destruct(p, T);
    return NX_DEFAULT_ALLOC::realloc(p, size);
}

template <typename AllocT, typename T>
inline pvoid realloc(T* p, size_t size)
{
    if (p) nx_destruct(p, T);
    return AllocT::realloc(p, size);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
