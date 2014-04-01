/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/fixed_pool.h"
#include "nixycore/memory/pool_center.h"

#include "nixycore/bugfix/assert.h"

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
class mem_pool : pool_center<StackT<AllocT> >
{
    typedef pool_center<StackT<AllocT> > base_t;
    typedef typename base_t::pool_t pool_t;

    struct alloc_t { size_t size_; };

    pool_t* find_pool(size_t size)
    {
        return base_t::acquire_pool(size, sizeof(alloc_t));
    }

    pvoid    cast(alloc_t* p) { return static_cast<pvoid>(p + 1); }
    alloc_t* cast(pvoid p)    { return static_cast<alloc_t*>(p) - 1; }

    pvoid do_alloc(size_t size)
    {
        pool_t* pool = find_pool(size);
        alloc_t* alc_p = (alloc_t*)(pool ? pool->alloc() : AllocT::alloc(sizeof(alloc_t) + size));
        nx_assert(alc_p);
        alc_p->size_ = size;
        return cast(alc_p);
    }

    void do_free(pvoid p)
    {
        alloc_t* alc_p = cast(p);
        pool_t* pool = find_pool(alc_p->size_);
        if (pool)
            pool->free(alc_p);
        else
            AllocT::free(alc_p);
    }

    alloc_t* mem_move(alloc_t* dst, alloc_t* src, pool_t* src_pool)
    {
        nx_assert(src_pool);
        memcpy(dst + 1, src + 1, src->size_);
        src_pool->free(src);
        return dst;
    }

    pvoid do_realloc(pvoid p, size_t size)
    {
        alloc_t* old_p = cast(p);
        pool_t* old_pool = find_pool(old_p->size_);
        pool_t* new_pool = find_pool(size);
        if (new_pool && !old_pool) return p;
        if (new_pool)
        {
            /* (old_pool != NULL) */
            if (old_pool->block_size() >= new_pool->block_size()) return p;
            /* (old_pool != new_pool) */
            alloc_t* new_p = (alloc_t*)new_pool->alloc();
            nx_assert(new_p);
            new_p->size_ = size;
            return cast(mem_move(new_p, old_p, old_pool));
        }
        /* (new_pool == NULL) */
        if (old_pool)
        {
            alloc_t* new_p = (alloc_t*)AllocT::alloc(sizeof(alloc_t) + size);
            nx_assert(new_p);
            new_p->size_ = size;
            return cast(mem_move(new_p, old_p, old_pool));
        }
        /* (new_pool == old_pool == NULL) */
        return cast((alloc_t*)AllocT::realloc(old_p, sizeof(alloc_t) + size));
    }

public:
    pvoid alloc(size_t size)
    {
        if (size == 0) return nx::nulptr;
        return do_alloc(size);
    }

    void free(pvoid p)
    {
        if (p) do_free(p);
    }

    pvoid realloc(pvoid p, size_t size)
    {
        if (p)
        {
            if (size == 0)
            {
                free(p);
                return nx::nulptr;
            }
            return do_realloc(p, size);
        }
        return alloc(size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
