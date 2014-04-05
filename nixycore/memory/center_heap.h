/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/cache_pool.h"
#include "nixycore/memory/mem_pool.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/typemanip/typedefs.h"
#include "nixycore/thread/thread_model.h"
#include "nixycore/pattern/singleton.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    center heap alloc policy model
*/

template <class AllocT = use::alloc_std>
struct center_heap_model
{
    typedef cache_pool<AllocT, NX_DEFAULT_THREAD_MODEL> cache_pool_t;
    typedef mem_pool<cache_pool_t> mem_pool_t;

    static mem_pool_t& instance(void)
    {
        static mem_pool_t& cache = nx::singleton<mem_pool_t>();
        return cache;
    }

    static pvoid alloc(size_t size)
    {
        return instance().alloc(size);
    }

    static void free(pvoid p, size_t size)
    {
        instance().free(p, size);
    }

    static pvoid realloc(pvoid p, size_t old_size, size_t new_size)
    {
        return instance().realloc(p, old_size, new_size);
    }
};

namespace use
{
    typedef alloc_model<center_heap_model<> > alloc_center_heap;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
