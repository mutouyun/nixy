/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Scoped Memory Pool
*/

template <class CacheT>
class mem_pool : CacheT
{
    typedef CacheT base_t;

    typedef typename base_t::alloc_t    alloc_t;
    typedef typename base_t::lock_t     lock_t;
    typedef typename base_t::lock_guard lock_guard;
    typedef typename base_t::pool_t     pool_t;

private:
    using base_t::find_pool;

    pvoid do_alloc(size_t size)
    {
        pool_t* pool = find_pool(size);
        pvoid alc_p;
        if (pool)
        {
            lock_guard NX_UNUSED guard(pool->lc_);
            alc_p = (*pool)->alloc();
        }
        else
            alc_p = nx::alloc<alloc_t>(size);
        nx_assert(alc_p);
        return alc_p;
    }

    void do_free(pvoid p, size_t size)
    {
        pool_t* pool = find_pool(size);
        if (pool)
        {
            lock_guard NX_UNUSED guard(pool->lc_);
            (*pool)->free(p);
        }
        else
            nx::free<alloc_t>(p, size);
    }

    pvoid mem_move(pvoid dst, pvoid src, size_t src_size, pool_t* src_pool)
    {
        nx_assert(src_pool);
        memcpy(dst, src, src_size);
        {
            lock_guard NX_UNUSED guard(src_pool->lc_);
            (*src_pool)->free(src);
        }
        return dst;
    }

    pvoid do_realloc(pvoid old_p, size_t old_size, size_t new_size)
    {
        pool_t* old_pool = find_pool(old_size);
        pool_t* new_pool = find_pool(new_size);
        if (new_pool && !old_pool) return old_p;
        if (new_pool)
        {
            /* (old_pool != NULL) */
            if ((*old_pool)->block_size() >= (*new_pool)->block_size()) return old_p;
            /* (old_pool != new_pool) */
            pvoid new_p;
            {
                lock_guard NX_UNUSED guard(new_pool->lc_);
                new_p  = (*new_pool)->alloc();
            }
            nx_assert(new_p);
            return mem_move(new_p, old_p, old_size, old_pool);
        }
        /* (new_pool == NULL) */
        if (old_pool)
        {
            pvoid new_p = nx::alloc<alloc_t>(new_size);
            nx_assert(new_p)(new_size);
            return mem_move(new_p, old_p, old_size, old_pool);
        }
        /* (new_pool == old_pool == NULL) */
        return nx::realloc<alloc_t>(old_p, old_size, new_size);
    }

public:
    pvoid alloc(size_t size)
    {
        if (size == 0) return NULL;
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
                return NULL;
            }
            return do_realloc(p, old_size, new_size);
        }
        return alloc(new_size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
