/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/fixed_pool.h"

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/bugfix/assert.h"
#include "nixycore/thread/thread_model.h"
#include "nixycore/pattern/singleton.h"

#include "nixycore/general/general.h"

#include "detail/skip_array.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    pool stack base class
*/

template <class AllocT>
struct pool_stack_base
{
    typedef NX_DEFAULT_THREAD_MODEL::lock_t lock_t;

    lock_t lc_;

    class lock_guard
    {
        lock_t& lc_;

    public:
        explicit lock_guard(lock_t& lc)
            : lc_(lc) { lc_.lock(); }
        ~lock_guard() { lc_.unlock(); }
    };

    typedef fixed_pool<AllocT, use::pool_expand_keep> pool_t;

    struct node_t
    {
        pool_t* pool_;
        node_t* next_;
    };
};

/*
    pool stack policy model
*/

template
<
    class AllocT,
    template <class> class PolicyT
>
class pool_stack_model : public PolicyT<AllocT>
{
    typedef PolicyT<AllocT> base_t;
    typedef typename base_t::lock_guard lock_guard;
    using base_t::lc_;

public:
    typedef typename base_t::pool_t  pool_t;
    typedef typename base_t::node_t  node_t;
    typedef typename base_t::array_t array_t;

private:
    array_t pools_;

public:
    using base_t::init_array;

    pool_stack_model(void)
    {
        init_array(pools_);
    }
    /* No return memory back to system */

public:
    node_t* pop_node(size_t n, size_t size)
    {
        nx_assert(n < base_t::MAX_SIZE);
        // get the top node_t
        node_t* new_node;
        {
            lock_guard NX_UNUSED guard(lc_);
            node_t*(& node) = pools_[n]; // [n] may change pools_
            new_node = node;
            if (node) node = node->next_;
        }
        if (new_node)
        {
            new_node->next_ = 0;
            return new_node;
        }
        // alloc a new node_t
        new_node = nx::alloc<AllocT, node_t>();
        nx_assert(new_node);
        new_node->pool_ = nx::alloc<AllocT, pool_t>(size);
        new_node->next_ = 0;
        return new_node;
    }

    void push_node(size_t n, node_t* new_node)
    {
        if (!new_node) return;
        nx_assert(new_node->pool_);
        nx_assert(n < base_t::MAX_SIZE);
        // put the node_t back to pools_
        node_t*(& node) = pools_[n]; // No need lock
        lock_guard NX_UNUSED guard(lc_);
        new_node->next_ = node;
        node = new_node;
    }

    void push_all(const array_t& pools)
    {
        base_t::for_each(pools, this, &pool_stack_model::push_node_p);
    }

    pool_t* locate_pool(const array_t& pools, pvoid p) const
    {
        if (!p) return 0;
        return static_cast<pool_t*>(base_t::for_each(pools, p, &pool_stack_model::has_block_p));
    }

private:
    static pvoid push_node_p(size_t n, node_t* new_node, pool_stack_model* this_p)
    {
        nx_assert(this_p);
        this_p->push_node(n, new_node);
        return 0;
    }

    static pvoid has_block_p(size_t /*n*/, node_t* node, pvoid p)
    {
        if (!node) return 0;
        pool_t* pool = node->pool_;
        nx_assert(pool);
        return pool->has_block(p) ? pool : 0;
    }
};

//////////////////////////////////////////////////////////////////////////

/*
    Define pool stack policy
*/

#ifndef NX_POOLSTACK_LEVELSIZ
#define NX_POOLSTACK_LEVELSIZ   (32)                // The size of every level
#endif

#ifndef NX_POOLSTACK_LEVELINC
#define NX_POOLSTACK_LEVELINC   sizeof(nx::pvoid)   // The increment of every level
#endif

#ifndef NX_POOLSTACK_LEVELNUM
#define NX_POOLSTACK_LEVELNUM   (2)                 // The number of levels
#endif

namespace use
{
    /*
        Using skip_array for policy storage
    */

    template <class AllocT>
    struct pool_stack_skip : pool_stack_base<AllocT>
    {
        typedef pool_stack_base<AllocT> base_t;

        NX_STATIC_PROPERTY(size_t, SMALL_SIZ, NX_POOLSTACK_LEVELSIZ);
        NX_STATIC_PROPERTY(size_t, SMALL_INC, NX_POOLSTACK_LEVELINC);

        typedef typename base_t::node_t node_t;
        typedef skip_array<node_t*, SMALL_SIZ, NX_POOLSTACK_LEVELNUM, AllocT> array_t;

        NX_STATIC_PROPERTY(size_t, MAX_SIZE, array_t::MAX);
        NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

        static size_t calculate_index(size_t size, size_t* size_ptr = 0)
        {
            size_t n;
            if (size <= MAX_SIZE * SMALL_INC)
            {
                n = ((size - 1) / SMALL_INC);
                if (size_ptr) (*size_ptr) = (n + 1) * SMALL_INC;
            }
            else
                return ERROR_INDEX;
            return n;
        }

        static void init_array(array_t& /*pools*/)
        {
            // Do nothing
        }

        template <class C, typename F>
        static pvoid for_each(const array_t& pools, C* wp, F do_something)
        {
            nx_auto(ite, pools.begin());
            for (; ite != pools.end(); ++ite)
            {
                pvoid p = do_something(ite.index(), *ite, wp);
                if (p) return p;
            }
            return 0;
        }
    };

    /*
        Using C-style array for policy storage
    */

    template <class AllocT>
    struct pool_stack_array : pool_stack_base<AllocT>
    {
        typedef pool_stack_base<AllocT> base_t;

        NX_STATIC_PROPERTY(size_t, SMALL_SIZ, NX_POOLSTACK_LEVELSIZ);
        NX_STATIC_PROPERTY(size_t, SMALL_INC, NX_POOLSTACK_LEVELINC);
        NX_STATIC_PROPERTY(size_t, SMALL_MAX, SMALL_SIZ * SMALL_INC);

        NX_STATIC_PROPERTY(size_t, LARGE_SIZ, NX_POOLSTACK_LEVELSIZ);
        NX_STATIC_PROPERTY(size_t, LARGE_INC, SMALL_INC << 1);
        NX_STATIC_PROPERTY(size_t, LARGE_MAX, LARGE_SIZ * LARGE_INC);

        NX_STATIC_PROPERTY(size_t, MAX_SIZE, SMALL_SIZ + LARGE_SIZ - 1);
        NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

        typedef typename base_t::node_t node_t;
        typedef node_t* (array_t[MAX_SIZE]);

        static size_t calculate_index(size_t size, size_t* size_ptr = 0)
        {
            size_t n;
            if (size <= SMALL_MAX)
            {
                n = ((size - 1) / SMALL_INC);
                if (size_ptr) (*size_ptr) = (n + 1) * SMALL_INC;
            }
            else
            if (size <= LARGE_MAX)
            {
                size -= SMALL_MAX;
                n = ((size - 1) / LARGE_INC) + SMALL_SIZ;
                if (size_ptr) (*size_ptr) = (n - SMALL_SIZ + 1) * LARGE_INC + SMALL_MAX;
            }
            else
                return ERROR_INDEX;
            return n;
        }

        static void init_array(array_t& pools)
        {
            memset(pools, 0, sizeof(pools));
        }

        template <class C, typename F>
        static pvoid for_each(const array_t& pools, C* wp, F do_something)
        {
            for (size_t n = 0; n < MAX_SIZE; ++n)
            {
                pvoid p = do_something(n, pools[n], wp);
                if (p) return p;
            }
            return 0;
        }
    };
}

//////////////////////////////////////////////////////////////////////////

/*
    Get a fixed pool from here
*/

template
<
    typename AllocT,
    template <class> class StackT
>
class pool_center : noncopyable
{
public:
    typedef pool_stack_model<AllocT, StackT> stack_t;

    typedef typename stack_t::node_t  node_t;
    typedef typename stack_t::pool_t  pool_t;
    typedef typename stack_t::array_t array_t;

private:
    stack_t& stack_;
    array_t  pools_;
    pool_t*  prev_pool_;

protected:
    pool_center(void)
        : stack_(singleton<stack_t>())
        , prev_pool_(0)
    {
        stack_.init_array(pools_);
    }

    ~pool_center(void)
    {
        stack_.push_all(pools_);
    }

    pool_t* acquire_pool(size_t size, size_t head_size = 0)
    {
        size_t n = stack_t::calculate_index(size, &size);
        if (n == stack_t::ERROR_INDEX) return 0;
        size += head_size;
        if (prev_pool_ && prev_pool_->block_size() == size) return prev_pool_;
        node_t*(&node) = pools_[n];
        if (node) return prev_pool_ = node->pool_;
        nx_verify(node = stack_.pop_node(n, size));
        return prev_pool_ = node->pool_;
    }

    pool_t* locate_pool(pvoid p)
    {
        if (prev_pool_ && prev_pool_->has_block(p)) return prev_pool_;
        return prev_pool_ = stack_.locate_pool(pools_, p);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
