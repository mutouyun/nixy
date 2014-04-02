/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/fixed_pool.h"
#include "nixycore/memory/pool_center.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Scoped Memory Pool
*/

#ifndef NX_MEMPOOL_STACK
#define NX_MEMPOOL_STACK    nx::use::pool_stack_skip
#endif

template
<
    typename AllocT = nx::use::alloc_std,

    template <class>
    class StackT = NX_MEMPOOL_STACK
>
class mem_pool : pool_center<AllocT, StackT>
{
    typedef pool_center<AllocT, StackT> base_t;
    typedef typename base_t::pool_t pool_t;

    typedef void alloc_t;
    NX_STATIC_PROPERTY(size_t, HEAD_SIZE, 0);

    pool_t* find_pool(size_t size)
    {
        return base_t::acquire_pool(size, HEAD_SIZE);
    }

    static pvoid void_cast(alloc_t* p)  { return p; }
    static alloc_t* alloc_cast(pvoid p) { return p; }

    pvoid do_alloc(size_t size)
    {
        pool_t* pool = find_pool(size);
        alloc_t* alc_p = (alloc_t*)(pool ? pool->alloc() : nx::alloc<AllocT>(HEAD_SIZE + size));
        nx_assert(alc_p);
        return void_cast(alc_p);
    }

    void do_free(pvoid p, size_t size)
    {
        alloc_t* alc_p = alloc_cast(p);
        pool_t* pool = find_pool(size);
        if (pool)
            pool->free(alc_p);
        else
            nx::free<AllocT>(alc_p, HEAD_SIZE + size);
    }

    alloc_t* mem_move(alloc_t* dst, alloc_t* src, size_t src_size, pool_t* src_pool)
    {
        nx_assert(src_pool);
        memcpy(void_cast(dst), void_cast(src), src_size);
        src_pool->free(src);
        return dst;
    }

    pvoid do_realloc(pvoid p, size_t old_size, size_t new_size)
    {
        alloc_t* old_p = alloc_cast(p);
        pool_t* old_pool = find_pool(old_size);
        pool_t* new_pool = find_pool(new_size);
        if (new_pool && !old_pool) return p;
        if (new_pool)
        {
            /* (old_pool != NULL) */
            if (old_pool->block_size() >= new_pool->block_size()) return p;
            /* (old_pool != new_pool) */
            alloc_t* new_p = (alloc_t*)new_pool->alloc();
            nx_assert(new_p);
            return void_cast(mem_move(new_p, old_p, old_size, old_pool));
        }
        /* (new_pool == NULL) */
        if (old_pool)
        {
            alloc_t* new_p = (alloc_t*)nx::alloc<AllocT>(HEAD_SIZE + new_size);
            nx_assert(new_p);
            return void_cast(mem_move(new_p, old_p, old_size, old_pool));
        }
        /* (new_pool == old_pool == NULL) */
        return void_cast((alloc_t*)nx::realloc<AllocT>(old_p, HEAD_SIZE + old_size, HEAD_SIZE + new_size));
    }

public:
    pvoid alloc(size_t size)
    {
        if (size == 0) return 0;
        return do_alloc(size);
    }

    void free(pvoid p, size_t size)
    {
        if (p) do_free(p, size);
    }

    pvoid realloc(pvoid p, size_t old_size, size_t new_size)
    {
        if (p && old_size)
        {
            if (new_size == 0)
            {
                free(p, old_size);
                return 0;
            }
            return do_realloc(p, old_size, new_size);
        }
        return alloc(new_size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
