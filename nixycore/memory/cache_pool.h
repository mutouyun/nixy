/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/fixed_pool.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typeof.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/thread/thread_model.h"

#include "nixycore/general/general.h"

#include "detail/skip_array.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Locate the index
*/

template <size_t LevelSizN, size_t SmallIncN>
struct cache_locator
{
    NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

    static const size_t SMALL_SIZE   = (LevelSizN * (SmallIncN));
    static const size_t LARGE_SIZE_0 = (LevelSizN * (SmallIncN << 1)) + SMALL_SIZE;
    static const size_t LARGE_SIZE_1 = (LevelSizN * (SmallIncN << 2)) + LARGE_SIZE_0;
    static const size_t LARGE_SIZE_2 = (LevelSizN * (SmallIncN << 3)) + LARGE_SIZE_1;

    // small block, size <= (LevelSizN * SmallIncN)
    inline static size_t calculate_index_small(size_t size, size_t* size_ptr)
    {
        size_t n = (size - 1) / SmallIncN;
        if (size_ptr) (*size_ptr) = (n + 1) * SmallIncN;
        return n;
    }

    // large block, size <= (LevelSizN * SmallIncN) * (2 ^ 4 - 1)
    inline static size_t calculate_index_large(size_t size, size_t* size_ptr)
    {
        if (size <= LARGE_SIZE_0)
        {
            size_t n = (size - SMALL_SIZE - 1) / (SmallIncN << 1);
            if (size_ptr) (*size_ptr) = (n + 1) * (SmallIncN << 1) + SMALL_SIZE;
            return n + LevelSizN;
        }
        else
        if (size <= LARGE_SIZE_1)
        {
            size_t n = (size - LARGE_SIZE_0 - 1) / (SmallIncN << 2);
            if (size_ptr) (*size_ptr) = (n + 1) * (SmallIncN << 2) + LARGE_SIZE_0;
            return n + LevelSizN * 2;
        }
        else
        {
            size_t n = (size - LARGE_SIZE_1 - 1) / (SmallIncN << 3);
            if (size_ptr) (*size_ptr) = (n + 1) * (SmallIncN << 3) + LARGE_SIZE_1;
            return n + LevelSizN * 3;
        }
    }

    // huge block, size > (LevelSizN * SmallIncN) * (2 ^ 4 - 1)
    inline static size_t calculate_index_huge(size_t size, size_t* size_ptr)
    {
        size_t prev_max = LARGE_SIZE_2;
        size_t curr_inc = SmallIncN << 4;
        size_t curr_max = LevelSizN * curr_inc + prev_max;
        for(size_t i = 4; i < LevelSizN; ++i, curr_inc <<= 1, curr_max += LevelSizN * curr_inc)
        {
            if (size <= curr_max)
            {
                size_t n = (size - prev_max - 1) / curr_inc;
                if (size_ptr) (*size_ptr) = (n + 1) * curr_inc + prev_max;
                return n + LevelSizN * i;
            }
            prev_max = curr_max;
        }
        return ERROR_INDEX;
    }

    inline static size_t calculate_index(size_t size, size_t* size_ptr = 0)
    {
        if (size <= SMALL_SIZE)
            return calculate_index_small(size, size_ptr);
        else
        if (size <= LARGE_SIZE_2)
            return calculate_index_large(size, size_ptr);
        else
            return calculate_index_huge(size, size_ptr);
    }
};

/*
    put pool into cache
*/

namespace private_cache_placer
{
    template <class LockT>
    class lock_guard
    {
        LockT& lc_;

    public:
        explicit lock_guard(LockT& lc)
            : lc_(lc) { lc_.lock(); }
        ~lock_guard() { lc_.unlock(); }
    };
}

template <class AllocT, class ModelT>
struct cache_placer
{
    typedef typename ModelT::lock_t lock_t;
    typedef private_cache_placer::lock_guard<lock_t> lock_guard;

private:
    lock_t lc_cache_;
    lock_t lc_alloc_;

public:
    template <typename R, class CacheT>
    R* acquire(CacheT& cache, size_t n, size_t size, size_t init_size)
    {
        typedef typename ModelT::template atomic<R**>::type_t pool_ptr_t;
        // get the current pool from cache
        pool_ptr_t pool_ptr;
        {
            lock_guard NX_UNUSED guard(lc_cache_);
            pool_ptr = &(cache[n]); // [n] may change the cache
        }
        nx_assert(pool_ptr);
        R*(& ret) = (*(pool_ptr.get()));
        if (*pool_ptr) return ret;
        // alloc a new pool, and put it into cache
        {
            lock_guard NX_UNUSED guard(lc_alloc_);
            if (!ret) (*pool_ptr) = nx::alloc<AllocT, R>(size, init_size);
        }
        return ret;
    }
};

template <class AllocT>
struct cache_placer<AllocT, use::thread_single>
{
    typedef typename use::thread_single::lock_t lock_t;
    typedef private_cache_placer::lock_guard<lock_t> lock_guard;

    template <typename R, class CacheT>
    R* acquire(CacheT& cache, size_t n, size_t size, size_t init_size)
    {
        // get the current pool from cache
        R*(& ret) = cache[n];
        if (ret) return ret;
        // alloc a new pool, and put it into cache
        return ret = nx::alloc<AllocT, R>(size, init_size);
    }
};

/*
    Caching fixed pools
*/

#ifndef NX_CACHEPOOL_INITSIZE   // The init memory size of every pool
#define NX_CACHEPOOL_INITSIZE   (sizeof(nx::pvoid) * 64)
#endif

#ifndef NX_CACHEPOOL_LEVELSIZ   // The size of every level
#define NX_CACHEPOOL_LEVELSIZ   (64)
#endif

#ifndef NX_CACHEPOOL_SMALLINC   // The increment of every level
#define NX_CACHEPOOL_SMALLINC   (sizeof(nx::pvoid))
#endif

template
<
    class AllocT, class ModelT,

    class FixAllocT = AllocT,

    template <class, size_t, template <typename, size_t> class>
    class FixExpandT = NX_FIXEDPOOL_EXPAND,

    template <typename, size_t>
    class FixModelT = use::iter_const,

    size_t InitSizeN = NX_CACHEPOOL_INITSIZE,
    size_t LevelSizN = NX_CACHEPOOL_LEVELSIZ,
    size_t SmallIncN = NX_CACHEPOOL_SMALLINC
>
class cache_pool : cache_placer<AllocT, ModelT>, noncopyable
{
public:
    typedef AllocT alloc_t;

    typedef cache_locator<LevelSizN, SmallIncN> locator_t;

    typedef cache_placer<AllocT, ModelT> placer_t;
    typedef typename placer_t::lock_t lock_t;
    typedef typename placer_t::lock_guard lock_guard;

    NX_STATIC_PROPERTY(size_t, ERROR_INDEX, locator_t::ERROR_INDEX);

    typedef fixed_pool<FixAllocT, FixExpandT, FixModelT, NX_FIXEDPOOL_ITERCOUNT> fixed_pool_t;

    struct pool_t
    {
        fixed_pool_t pool_;
        lock_t lc_;

        pool_t(size_t s, size_t c) : pool_(s, c) {}

        fixed_pool_t*       operator->(void)       { return &pool_; }
        const fixed_pool_t* operator->(void) const { return &pool_; }
    };

    typedef skip_array<pool_t*, LevelSizN, 2, alloc_t> cache_t;

    template <class C, typename F>
    pvoid for_each(C* wp, F do_sth)
    {
        nx_auto(ite, cache_.begin());
        for (; ite != cache_.end(); ++ite)
        {
            pvoid p = do_sth(ite.index(), *ite, wp);
            if (p) return p;
        }
        return 0;
    }

private:
    cache_t cache_;

    static pvoid destroy_p(size_t /*n*/, pool_t* pool_ptr, pvoid /*p*/)
    {
        nx::free<alloc_t>(pool_ptr);
        return 0;
    }

    static pvoid has_block_p(size_t /*n*/, pool_t* pool_ptr, pvoid p)
    {
        if (!pool_ptr) return 0;
        return pool_ptr->has_block(p) ? pool_ptr : 0;
    }

public:
    ~cache_pool(void)
    {
        for_each((pvoid)0, &cache_pool::destroy_p);
    }

    pool_t* find_pool(size_t size)
    {
        // check, and get pool index and block size
        size_t n = locator_t::calculate_index(size, &size);
        if (n == ERROR_INDEX) return 0;
        // get a matched pool from cache_
        return placer_t::template acquire<pool_t>(cache_, n, size, InitSizeN / size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
