/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/construct.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    construct alloc
*/

namespace private_alloc
{
    template <class AllocT, typename T>
    struct detail
    {
#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        static T* alloc(nx_fref(P)... par)
        {
            return nx_construct(AllocT::alloc(sizeof(T)), T, (nx_forward(P, par)...));
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        static T* alloc(void)
        {
            return nx_construct(AllocT::alloc(sizeof(T)), T);
        }

#   define NX_ALLOC_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        static T* alloc(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            return nx_construct(AllocT::alloc(sizeof(T)), T, (NX_PP_FORWARD(n, P, par))); \
        }
        NX_PP_MULT_MAX(NX_ALLOC_)
#   undef NX_ALLOC_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    /* Make array to array pointer */
    template <class AllocT, typename T_, size_t N>
    struct detail<AllocT, T_[N]>
    {
        typedef T_ T[N];
#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        static T* alloc(nx_fref(P)... par)
        {
            T* p = (T*)AllocT::alloc(sizeof(T));
            nx_construct_arr(*p, T_, N, (nx_forward(P, par)...));
            return p;
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        static T* alloc(void)
        {
            T* p = (T*)AllocT::alloc(sizeof(T));
            nx_construct_arr(*p, T_, N);
            return p;
        }

#   define NX_ALLOC_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        static T* alloc(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            T* p = (T*)AllocT::alloc(sizeof(T)); \
            nx_construct_arr(*p, T_, N, (NX_PP_FORWARD(n, P, par))); \
            return p; \
        }
        NX_PP_MULT_MAX(NX_ALLOC_)
#   undef NX_ALLOC_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <class AllocT>
    struct detail<AllocT, void>
    {
        static void* alloc(size_t size)
        {
            return AllocT::alloc(size);
        }
    };
}

template <class AllocT>
inline pvoid alloc(size_t size)
{
    return AllocT::alloc(size);
}

#ifdef NX_SP_CXX11_TEMPLATES
template <class AllocT, typename T, typename... P>
inline T* alloc(nx_fref(P)... par)
{
    return private_alloc::detail<AllocT, T>::alloc(nx_forward(P, par)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <class AllocT, typename T>
inline T* alloc(void)
{
    return private_alloc::detail<AllocT, T>::alloc();
}

#define NX_ALLOC_(n) \
template <class AllocT, typename T, NX_PP_TYPE_1(n, typename P)> \
inline T* alloc(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return private_alloc::detail<AllocT, T>::alloc(NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_ALLOC_)
#undef NX_ALLOC_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    destruct free
*/

template <class AllocT>
inline void free(pvoid p, size_t size)
{
    AllocT::free(p, size);
}

// make a concept to check size_of function

NX_CONCEPT(size_of_normal, size_t, size_of, )
NX_CONCEPT(size_of_const , size_t, size_of, const)

// has no virtual-destructor or size_of function

template <class AllocT, typename T>
inline typename nx::enable_if<!nx::has_virtual_destructor<T>::value ||
                             (!nx::has_size_of_normal<T>::value &&
                              !nx::has_size_of_const<T>::value)
>::type_t free(T* p)
{
    if (!p) return;
    nx_destruct(p, T);
    AllocT::free(p, sizeof(T));
}

// has virtual-destructor and size_of function

template <class AllocT, typename T>
inline typename nx::enable_if<nx::has_virtual_destructor<T>::value &&
                             (nx::has_size_of_normal<T>::value ||
                              nx::has_size_of_const<T>::value)
>::type_t free(T* p)
{
    if (!p) return;
    size_t s = p->size_of();
    nx_destruct(p, T);
    AllocT::free(p, s);
}

// array

template <class AllocT, typename T, size_t N>
inline void free(T(* p)[N])
{
    if (!p) return;
    nx_destruct_arr(*p, T, N);
    AllocT::free(p, sizeof(T[N]));
}

/*
    realloc
*/

template <class AllocT>
inline pvoid realloc(pvoid p, size_t old_size, size_t new_size)
{
    return AllocT::realloc(p, old_size, new_size);
}

// has no virtual-destructor or size_of function

template <class AllocT, typename T>
inline typename nx::enable_if<!nx::has_virtual_destructor<T>::value ||
                             (!nx::has_size_of_normal<T>::value &&
                              !nx::has_size_of_const<T>::value),
pvoid>::type_t realloc(T* p, size_t size)
{
    if (p) nx_destruct(p, T);
    return AllocT::realloc(p, sizeof(T), size);
}

// has virtual-destructor and size_of function

template <class AllocT, typename T>
inline typename nx::enable_if<nx::has_virtual_destructor<T>::value &&
                             (nx::has_size_of_normal<T>::value ||
                              nx::has_size_of_const<T>::value),
pvoid>::type_t realloc(T* p, size_t size)
{
    size_t s = 0;
    if (p)
    {
        s = p->size_of();
        nx_destruct(p, T);
    }
    return AllocT::realloc(p, s, size);
}

/*
    define macros
*/

#ifndef nx_alloc
#define nx_alloc    nx::alloc
#endif

#ifndef nx_free
#define nx_free     nx::free
#endif

#ifndef nx_realloc
#define nx_realloc  nx::realloc
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
