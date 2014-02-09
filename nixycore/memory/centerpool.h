/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/fixedpool.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/thread/threadmodel.h"
#include "nixycore/pattern/singleton.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Fixed Pool Stack
*/

template <class Alloc_>
class by_pool_stack_fixed
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

public:
    typedef fixed_pool<Alloc_, by_pool_expand_keep> pool_t;

    NX_STATIC_PROPERTY(size_t, SMALL_NUM, 64);
    NX_STATIC_PROPERTY(size_t, SMALL_INC, sizeof(pvoid));
    NX_STATIC_PROPERTY(size_t, SMALL_SIZ, SMALL_NUM * SMALL_INC);

    NX_STATIC_PROPERTY(size_t, LARGE_NUM, 64);
    NX_STATIC_PROPERTY(size_t, LARGE_INC, SMALL_SIZ);
    NX_STATIC_PROPERTY(size_t, LARGE_SIZ, LARGE_NUM * LARGE_INC);

    NX_STATIC_PROPERTY(size_t, TOTAL_NUM, SMALL_NUM + LARGE_NUM - 1);

    NX_STATIC_PROPERTY(size_t, ERROR_INDEX, -1);

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
            n = ((size - 1) / LARGE_INC) + SMALL_NUM - 1;
            if (size_ptr) (*size_ptr) = (n - SMALL_NUM + 2) * LARGE_INC;
        }
        else
            return ERROR_INDEX;
        return n;
    }

    struct node_t
    {
        pool_t* pool_;
        node_t* next_;
    };

private:
    node_t* (pools_[TOTAL_NUM]);

public:
    by_pool_stack_fixed(void)
    {
        std::memset(pools_, 0, sizeof(pools_));
    }

public:
    node_t* pop_node(size_t n, size_t size)
    {
        nx_assert(n < nx_countof(pools_));
        // get the top node_t
        node_t*(& node) = pools_[n];
        node_t* new_node;
        {
            lock_guard guard(lc_);
            new_node = node;
            if (node) node = node->next_;
        }
        if (new_node)
        {
            new_node->next_ = nx::nulptr;
            return new_node;
        }
        // alloc a new node_t
        new_node = static_cast<node_t*>(Alloc_::alloc(sizeof(node_t)));
        nx_assert(new_node);
        new_node->pool_ = nx_construct(Alloc_::alloc(sizeof(pool_t)), pool_t, size);
        new_node->next_ = nx::nulptr;
        return new_node;
    }

    void push_node(size_t n, node_t* new_node)
    {
        if (!new_node) return;
        nx_assert(new_node->pool_);
        nx_assert(n < nx_countof(pools_));
        // put the node_t back to pools_
        node_t*(& node) = pools_[n];
        lock_guard guard(lc_);
        new_node->next_ = node;
        node = new_node;
    }
};

/*
    Center Memory Pool, get a fixed pool from here
*/

template <class Stack_>
class CenterPool : Stack_, NonCopyable
{
public:
    typedef Stack_ stack_t;

    typedef typename stack_t::node_t node_t;
    typedef typename stack_t::pool_t pool_t;

private:
    stack_t& stack_;
    node_t* (pools_[stack_t::TOTAL_NUM]);

protected:
    CenterPool(void)
        : stack_(singleton<stack_t>())
    {
        std::memset(pools_, 0, sizeof(pools_));
    }

    ~CenterPool()
    {
        for(size_t n = 0; n < nx_countof(pools_); ++n)
            stack_.push_node(n, pools_[n]);
    }

    pool_t* find_pool(size_t size, size_t head_size = 0)
    {
        size_t n = stack_t::calculate_index(size, &size);
        if (n == stack_t::ERROR_INDEX) return nx::nulptr;
        node_t* (&node) = pools_[n];
        if (node) return node->pool_;
        nx_verify(node = stack_.pop_node(n, head_size + size));
        return node->pool_;
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
