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

// nx::skip_array
#include "detail/skip_array.h"
// std::pair
#include <utility>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Locate the index
*/

namespace private_cache_locator
{
    /* X raised by the exponent of Y */

    template <size_t X, size_t Y>
    struct exp
    {
        NX_STATIC_VALUE(size_t, X * exp<X, Y - 1>::value);
    };

    template <size_t X>
    struct exp<X, 1>
    {
        NX_STATIC_VALUE(size_t, X);
    };

    template <size_t X>
    struct exp<X, 0>
    {
        NX_STATIC_VALUE(size_t, 1);
    };
}

template <size_t ClassNumN, size_t LevelNumN, size_t SmallIncN, size_t FactorN>
struct cache_locator
{
    template <size_t X, size_t Y>
    struct exp : private_cache_locator::exp<X, Y> {};

    static const size_t X = (1 << FactorN);

    static const size_t INC_SIZE   = (SmallIncN * exp<X, 1>::value);
    static const size_t INC_SIZE_0 = (SmallIncN * exp<X, 2>::value);
    static const size_t INC_SIZE_1 = (SmallIncN * exp<X, 3>::value);
    static const size_t INC_SIZE_2 = (SmallIncN * exp<X, 4>::value);

    static const size_t LEV_SIZE   = (LevelNumN * SmallIncN);
    static const size_t LEV_SIZE_0 = (LevelNumN * INC_SIZE  ) + LEV_SIZE;
    static const size_t LEV_SIZE_1 = (LevelNumN * INC_SIZE_0) + LEV_SIZE_0;
    static const size_t LEV_SIZE_2 = (LevelNumN * INC_SIZE_1) + LEV_SIZE_1;

    typedef std::pair<size_t, size_t> pair_t;

    inline static pair_t calculate_index(size_t size, size_t* size_ptr = 0)
    {
        if (size <= LEV_SIZE)
            return calculate_index_small(size, size_ptr);
        else
        if (size <= LEV_SIZE_2)
            return calculate_index_large(size, size_ptr);
        else
            return calculate_index_huge(size, size_ptr);
    }

    // small block, size <= (LevelNumN * SmallIncN)
    inline static pair_t calculate_index_small(size_t size, size_t* size_ptr)
    {
        size_t n = (size - 1) / SmallIncN;
        if (size_ptr) (*size_ptr) = (n + 1) * SmallIncN;
        return pair_t(0, n);
    }

    // large block, size <= (LevelNumN * SmallIncN) * (X ^ 4 - 1) / (X - 1)
    inline static pair_t calculate_index_large(size_t size, size_t* size_ptr)
    {
        if (size <= LEV_SIZE_0)
        {
            size_t n = (size - (LEV_SIZE + 1)) / INC_SIZE;
            if (size_ptr) (*size_ptr) = (n + 1) * INC_SIZE + LEV_SIZE;
            return pair_t(1, n);
        }
        else
        if (size <= LEV_SIZE_1)
        {
            size_t n = (size - (LEV_SIZE_0 + 1)) / INC_SIZE_0;
            if (size_ptr) (*size_ptr) = (n + 1) * INC_SIZE_0 + LEV_SIZE_0;
            return pair_t(2, n);
        }
        else
        {
            size_t n = (size - (LEV_SIZE_1 + 1)) / INC_SIZE_1;
            if (size_ptr) (*size_ptr) = (n + 1) * INC_SIZE_1 + LEV_SIZE_1;
            return pair_t(3, n);
        }
    }

    // huge block, size > (LevelNumN * SmallIncN) * (X ^ 4 - 1) / (X - 1)
    inline static pair_t calculate_index_huge(size_t size, size_t* size_ptr)
    {
        size_t prev_max = LEV_SIZE_2;
        size_t curr_inc = INC_SIZE_2;
        size_t curr_max = LevelNumN * curr_inc + prev_max;
        for(size_t i = 4; i < LevelNumN; ++i, curr_inc <<= FactorN, curr_max += LevelNumN * curr_inc)
        {
            if (size <= curr_max)
            {
                size_t n = (size - prev_max - 1) / curr_inc;
                if (size_ptr) (*size_ptr) = (n + 1) * curr_inc + prev_max;
                return pair_t(i, n);
            }
            prev_max = curr_max;
        }
        return pair_t(ClassNumN, 0);
    }
};

/*
    put pool into cache
*/

namespace private_cache_placer
{
    template <class AllocT>
    struct base
    {
        template <class LockT>
        class lock_guard
        {
            LockT& lc_;

        public:
            explicit lock_guard(LockT& lc)
                : lc_(lc) {
                lc_.lock();
            }
            ~lock_guard() { lc_.unlock(); }
        };

        template <class CacheT>
        void release(CacheT& cache)
        {
            nx_auto(ite, cache.begin());
            for(; ite != cache.end(); ++ite)
                nx::free<AllocT>(*ite);
        }
    };
}

template <class AllocT, class ModelT>
struct cache_placer : private_cache_placer::base<AllocT>
{
    typedef private_cache_placer::base<AllocT> base_t;
    typedef typename ModelT::lock_t lock_t;
    typedef typename base_t::template lock_guard<lock_t> lock_guard;

private:
    lock_t lc_cache_;
    lock_t lc_alloc_;

public:
    template <typename R, class CacheT>
    R* acquire(CacheT& cache, size_t n, size_t i, size_t size, size_t init_size)
    {
        typedef typename ModelT::template atomic<R**>::type_t pool_ptr_t;
        // get the current pool from cache
        pool_ptr_t pool_ptr;
        {
            lock_guard NX_UNUSED guard(lc_cache_);
            pool_ptr = &(cache.at(n, i)); // at() may change the cache
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
struct cache_placer<AllocT, use::thread_single> : private_cache_placer::base<AllocT>
{
    typedef private_cache_placer::base<AllocT> base_t;
    typedef typename use::thread_single::lock_t lock_t;
    typedef typename base_t::template lock_guard<lock_t> lock_guard;

    template <typename R, class CacheT>
    R* acquire(CacheT& cache, size_t n, size_t i, size_t size, size_t init_size)
    {
        // get the current pool from cache
        R*(& ret) = cache.at(n, i);
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

#ifndef NX_CACHEPOOL_CLASSNUM   // The number of memory block size class
#define NX_CACHEPOOL_CLASSNUM   (16)
#endif

#ifndef NX_CACHEPOOL_LEVELNUM   // The number of levels in one class
#define NX_CACHEPOOL_LEVELNUM   (64)
#endif

#ifndef NX_CACHEPOOL_SMALLINC   // The increment of every level
#define NX_CACHEPOOL_SMALLINC   (sizeof(nx::pvoid))
#endif

#ifndef NX_CACHEPOOL_FACTOR     // The incremental factor
#define NX_CACHEPOOL_FACTOR     (2)
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
    size_t ClassNumN = NX_CACHEPOOL_CLASSNUM,
    size_t LevelNumN = NX_CACHEPOOL_LEVELNUM,
    size_t SmallIncN = NX_CACHEPOOL_SMALLINC,
    size_t FactorN   = NX_CACHEPOOL_FACTOR
>
class cache_pool : cache_placer<AllocT, ModelT>, noncopyable
{
public:
    typedef AllocT alloc_t;

    typedef cache_locator<ClassNumN, LevelNumN, SmallIncN, FactorN> locator_t;
    typedef typename locator_t::pair_t pair_t;

    typedef cache_placer<AllocT, ModelT> placer_t;
    typedef typename placer_t::lock_t lock_t;
    typedef typename placer_t::lock_guard lock_guard;

    typedef fixed_pool<FixAllocT, FixExpandT, FixModelT, NX_FIXEDPOOL_ITERCOUNT> fixed_pool_t;

    struct pool_t
    {
        fixed_pool_t pool_;
        lock_t lc_;

        pool_t(size_t s, size_t c) : pool_(s, c) {}

        fixed_pool_t*       operator->(void)       { return &pool_; }
        const fixed_pool_t* operator->(void) const { return &pool_; }
    };

    typedef skip_array<pool_t*, ClassNumN, LevelNumN, alloc_t> cache_t;

private:
    cache_t cache_;

public:
    ~cache_pool(void)
    {
        placer_t::release(cache_);
    }

    pool_t* find_pool(size_t size)
    {
        // check, and get pool index and block size
        pair_t xx = locator_t::calculate_index(size, &size);
        if (xx.first == ClassNumN) return 0;
        // get a matched pool from cache_
        return placer_t::template acquire<pool_t>
            (cache_, xx.first, xx.second, size, InitSizeN / size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
