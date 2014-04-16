/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/mem_alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"

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
    return nx::alloc<NX_DEFAULT_ALLOC>(size);
}

#ifdef NX_SP_CXX11_TEMPLATES
template <typename T, typename... P>
inline typename nx::enable_if<!nx::is_alloc<T>::value,
T*>::type_t alloc(nx_fref(P, ... par))
{
    return nx::alloc<NX_DEFAULT_ALLOC, T>(nx_forward(P, par)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename T>
inline T* alloc(void)
{
    return nx::alloc<NX_DEFAULT_ALLOC, T>();
}

#define NX_ALLOC_(n) \
template <typename T, NX_PP_TYPE_1(n, typename P)> \
inline typename nx::enable_if<!nx::is_alloc<T>::value, \
T*>::type_t alloc(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return nx::alloc<NX_DEFAULT_ALLOC, T>(NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_ALLOC_)
#undef NX_ALLOC_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    destruct free
*/

inline void free(pvoid p, size_t size)
{
    nx::free<NX_DEFAULT_ALLOC>(p, size);
}

template <typename T>
inline void free(T* p)
{
    nx::free<NX_DEFAULT_ALLOC>(p);
}

/*
    realloc
*/

inline pvoid realloc(pvoid p, size_t old_size, size_t new_size)
{
    return nx::realloc<NX_DEFAULT_ALLOC>(p, old_size, new_size);
}

template <typename T>
inline pvoid realloc(T* p, size_t size)
{
    return nx::realloc<NX_DEFAULT_ALLOC>(p, size);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
