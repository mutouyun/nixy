/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "general/general.h"

// placement new, std::bad_alloc
#include <new>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    construct/destruct
*/

#define nx_construct(p, T, ...) \
    (T*)::new ((void*)(p)) T(__VA_ARGS__)

#define nx_construct_arr(p, T, N, ...) \
    do \
    { \
        for(size_t i = 0; i < N; ++i) \
            nx_construct(&(((T*)(p))[i]), T, __VA_ARGS__); \
    } while(false)

#define nx_destruct(p, T) \
    ((T*)(p))->~T()

#define nx_destruct_arr(p, T, N) \
    do \
    { \
        for(size_t i = 0; i < N; ++i) \
            nx_destruct(&(((T*)(p))[i]), T); \
    } while(false)

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
