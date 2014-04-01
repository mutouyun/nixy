/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/fixed_pool.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/thread/thread_model.h"
#include "nixycore/pattern/singleton.h"
#include "nixycore/algorithm/foreach.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

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

template <class AllocT, template <class> class PolicyT>
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

    node_t* pop_node(size_t n, size_t size)
    {
        nx_assert(n < base_t::MAX_SIZE);
        // get the top node_t
        node_t* new_node;
        {
            lock_guard NX_UNUSED guard(lc_);
            node_t*(& node) = pools_[n];
            new_node = node;
            if (node) node = node->next_;
        }
        if (new_node)
        {
            new_node->next_ = nx::nulptr;
            return new_node;
        }
        // alloc a new node_t
        new_node = static_cast<node_t*>(AllocT::alloc(sizeof(node_t)));
        nx_assert(new_node);
        new_node->pool_ = nx_construct(AllocT::alloc(sizeof(pool_t)), pool_t, size);
        new_node->next_ = nx::nulptr;
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

    pool_t* acquire_pool(array_t& pools, size_t size, size_t head_size)
    {
        size_t n = base_t::calculate_index(size, &size);
        if (n == base_t::ERROR_INDEX) return nx::nulptr;
        node_t*(&node) = pools[n];
        if (node) return node->pool_;
        nx_verify(node = pop_node(n, head_size + size));
        return node->pool_;
    }

    void release_all(const array_t& pools)
    {
        base_t::release_all(pools, this, &pool_stack_model::push_node);
    }
};

//////////////////////////////////////////////////////////////////////////

/*
    define pool stack policy
*/

#ifndef NX_POOLSTACK_LEVELNUM
#define NX_POOLSTACK_LEVELNUM   (32)
#endif

#ifndef NX_POOLSTACK_LEVELINC
#define NX_POOLSTACK_LEVELINC   sizeof(nx::pvoid)
#endif

namespace private_pool_stack
{
    /*
        Using skip_array for policy storage
    */

    template <class AllocT>
    struct model_skip : pool_stack_base<AllocT>
    {
        typedef pool_stack_base<AllocT> base_t;

        NX_STATIC_PROPERTY(size_t, SMALL_NUM, NX_POOLSTACK_LEVELNUM);
        NX_STATIC_PROPERTY(size_t, SMALL_INC, NX_POOLSTACK_LEVELINC);

        typedef typename base_t::node_t node_t;
        typedef skip_array<node_t*, SMALL_NUM, 2, AllocT> array_t;

        NX_STATIC_PROPERTY(size_t, MAX_SIZE, array_t::MAX);
        NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

        static size_t calculate_index(size_t size, size_t* size_ptr = nx::nulptr)
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
        static void release_all(const array_t& pools, C* wp, F push_node)
        {
            nx_auto(ite, pools.begin());
            for (; ite != pools.end(); ++ite)
                (wp->*push_node)(ite.index(), *ite);
        }
    };

    /*
        Using C-style array for policy storage
    */

    template <class AllocT>
    struct model_array : pool_stack_base<AllocT>
    {
        typedef pool_stack_base<AllocT> base_t;

        NX_STATIC_PROPERTY(size_t, SMALL_NUM, NX_POOLSTACK_LEVELNUM);
        NX_STATIC_PROPERTY(size_t, SMALL_INC, NX_POOLSTACK_LEVELINC);
        NX_STATIC_PROPERTY(size_t, SMALL_SIZ, SMALL_NUM * SMALL_INC);

        NX_STATIC_PROPERTY(size_t, LARGE_NUM, NX_POOLSTACK_LEVELNUM);
        NX_STATIC_PROPERTY(size_t, LARGE_INC, SMALL_SIZ >> 2);
        NX_STATIC_PROPERTY(size_t, LARGE_SIZ, LARGE_NUM * LARGE_INC);

        NX_STATIC_PROPERTY(size_t, MAX_SIZE, SMALL_NUM + LARGE_NUM - 1);
        NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

        typedef typename base_t::node_t node_t;
        typedef node_t* (array_t[MAX_SIZE]);

        static size_t calculate_index(size_t size, size_t* size_ptr = nx::nulptr)
        {
            size_t n;
            if (size <= SMALL_SIZ)
            {
                n = ((size - 1) / SMALL_INC);
                if (size_ptr) (*size_ptr) = (n + 1) * SMALL_INC;
            }
            else
            if (size <= LARGE_SIZ)
            {
                size -= SMALL_SIZ;
                n = ((size - 1) / LARGE_INC) + SMALL_NUM;
                if (size_ptr) (*size_ptr) = (n - SMALL_NUM + 1) * LARGE_INC + SMALL_SIZ;
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
        static void release_all(const array_t& pools, C* wp, F push_node)
        {
            for (size_t n = 0; n < MAX_SIZE; ++n)
                (wp->*push_node)(n, pools[n]);
        }
    };
}

namespace use
{
    template <class AllocT>
    struct pool_stack_skip
         : pool_stack_model<AllocT, private_pool_stack::model_skip>
    {};

    template <class AllocT>
    struct pool_stack_array
         : pool_stack_model<AllocT, private_pool_stack::model_array>
    {};
}

//////////////////////////////////////////////////////////////////////////

/*
    Get a fixed pool from here
*/

template <class StackT>
class pool_center : noncopyable
{
public:
    typedef StackT stack_t;

    typedef typename stack_t::node_t  node_t;
    typedef typename stack_t::pool_t  pool_t;
    typedef typename stack_t::array_t array_t;

private:
    stack_t& stack_;
    array_t pools_;

protected:
    pool_center(void)
        : stack_(singleton<stack_t>())
    {
        stack_.init_array(pools_);
    }

    ~pool_center(void)
    {
        stack_.release_all(pools_);
    }

    pool_t* acquire_pool(size_t size, size_t head_size)
    {
        return stack_.acquire_pool(pools_, size, head_size);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
