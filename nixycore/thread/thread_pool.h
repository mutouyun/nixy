/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/thread_detail.h"
#include "nixycore/thread/blocking_queue.h"
#include "nixycore/thread/lock_guard.h"
#include "nixycore/thread/mutex.h"
#include "nixycore/thread/atomic.h"
#include "nixycore/thread/condition.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/memory/memory.h"

//////////////////////////////////////////////////////////////////////////
#include "nixycore/al/general/disable_warnings.hxx"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_thread_pool
{
    struct wrapper : public thread
    {
        pvoid prev_; // the previous thread node recording pointer
        bool is_exit_;
        mutex wait_;

        template <typename F, typename C>
        wrapper(F f, C c)
            : is_exit_(false)
        {
            wait_.lock();
            thread::start(f, c, this);
        }

        void start(void) { wait_.unlock(); }
        void wait(void)  { wait_.lock(); }
    };

    template <typename T, class FixedAlloc_>
    class storage : public object_pool_storage<T, FixedAlloc_>
    {
        typedef object_pool_storage<T, FixedAlloc_> base_t;
        typedef typename base_t::alloc_t alloc_t;

        using base_t::allocator_;
        using base_t::constructor_;
        using base_t::free_head_;
        using base_t::free_tail_;
        using base_t::size_;

    public:
        mutable mutex queue_lock_;
        mutable mutex alloc_lock_;
        condition until_empty_;

        typedef typename base_t::type_t type_t;
        nx_assert_static(is_sametype<type_t, wrapper>::value);

    public:
        template <typename F>
        explicit storage(const F& f)
            : base_t(f)
            , until_empty_(queue_lock_)
        {}

    public:
        type_t* create(void)
        {
            alloc_t* p;
            {
                nx_lock_scope(alloc_lock_);
                p = static_cast<alloc_t*>(allocator_.alloc());
            }
            nx_assert(p);
            return constructor_(p->data_);
        }

        void destroy(type_t* objc)
        {
            nx_assert(objc);
            objc->is_exit_ = true;
            nx_destruct(objc, type_t);
            {
                nx_lock_scope(alloc_lock_);
                allocator_.free(objc);
            }
            nx_lock_scope(queue_lock_);
            if (base_t::is_empty())
                until_empty_.notify();
        }

        type_t* take(void)
        {
            nx_lock_scope(queue_lock_);
            return base_t::take();
        }

        type_t* take(type_t* objc)
        {
            nx_assert(objc);
            nx_lock_scope(queue_lock_);
            nx_assert(free_head_);
            alloc_t* curr = reinterpret_cast<alloc_t*>(objc);
            if (curr == free_head_ && curr == free_tail_)
            {
                free_tail_ = free_head_ = nx::nulptr;
            }
            else
            if (curr == free_head_)
            {
                free_head_ = free_head_->next_;
            }
            else
            if (curr == free_tail_)
            {
                free_tail_ = static_cast<alloc_t*>(reinterpret_cast<type_t*>(free_tail_)->prev_);
            }
            else
            {
                alloc_t* prev = static_cast<alloc_t*>(objc->prev_);
                alloc_t* next = curr->next_;
                prev->next_ = next;
                reinterpret_cast<type_t*>(next)->prev_ = prev;
            }
            -- size_;
            return objc;
        }

        void put(type_t* objc)
        {
            nx_assert(objc);
            nx_lock_scope(queue_lock_);
            objc->prev_ = free_tail_;
            base_t::put(objc);
            objc->start();
        }

        size_t size(void) const
        {
            nx_lock_scope(queue_lock_);
            return base_t::size();
        }

        bool is_empty(void) const
        {
            nx_lock_scope(queue_lock_);
            return base_t::is_empty();
        }

        void clear(const functor<void(size_t)>& do_finish)
        {
            nx_lock_scope(queue_lock_);
            alloc_t* p = free_head_;
            for (size_t i = 0; i < size_; ++i)
            {
                reinterpret_cast<type_t*>(p)->is_exit_ = true;
                p = p->next_;
            }
            do_finish(base_t::size());
            while (!base_t::is_empty()) // used to avoid spurious wakeups
                until_empty_.wait();
        }
    };
}

class thread_pool : public object_pool<private_thread_pool::wrapper, fixed_pool<>, private_thread_pool::storage>
{
    typedef object_pool<private_thread_pool::wrapper, fixed_pool<>, private_thread_pool::storage> base_t;
    typedef base_t::type_t type_t;

    using base_t::storage_;

public:
    typedef type_t::task_t task_t;

private:
    blocking_queue<task_t> task_queue_;
    atomic<size_t> busy_count_;

    bool is_continue(type_t* tp)
    {
        if (tp->is_exit_)
            return false;
        if (base_t::is_full())
            return false;
        return true;
    }

    void set_busy(bool b)
    {
        busy_count_ += (b ? 1 : -1);
    }

    void remove(type_t* tp)
    {
        tp->detach();
        storage_.destroy(storage_.take(tp));
    }

    void onProcess(type_t* tp)
    {
        nx_assert(tp);
        tp->wait();
        while (is_continue(tp))
        {
            task_t task = task_queue_.take(); // wait for a new task
            if (task)
            {
                set_busy(true);
                task();
                set_busy(false);
            }
        }
        remove(tp);
    }

    void shock(size_t size)
    {
        for (size_t i = 0; i < size; ++i)
            task_queue_.put(nx::none); // put an empty task
    }

public:
    thread_pool(size_t min_sz = 0, size_t max_sz = 0)
        : base_t(&thread_pool::onProcess, this)
    {
        limit(min_sz, max_sz);
    }

    ~thread_pool()
    {
        storage_.clear(functor<void(size_t)>(&thread_pool::shock, this));
    }

public:
    void limit(size_t min_sz = 0, size_t max_sz = 0)
    {
        if (max_sz == 0)
        {
            max_sz = nx::detect_cpu_count();
            if (max_sz < min_sz)
                max_sz = min_sz;
        }
        nx_lock_scope(storage_.queue_lock_);
        base_t::limit(min_sz, max_sz);
    }

    template <typename F>
    void put(const F& f)
    {
        task_queue_.put(f);
        if (task_queue_.size() > idle_count())
            base_t::increase(); // try to expansion storage
    }

#define NX_THREAD_POOL_PUT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void put(const F& f, NX_PP_TYPE_2(n, P, par)) \
    { \
        put(bind<void>(f, NX_PP_TYPE_1(n, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_POOL_PUT_)
#undef NX_THREAD_POOL_PUT_

    size_t busy_count(void) const
    {
        return busy_count_;
    }

    size_t idle_count(void) const
    {
        return (storage_.size() - busy_count_);
    }

    bool wait_finish(int tm_ms = -1)
    {
        return task_queue_.wait_empty(tm_ms);
    }

private:
    using base_t::alloc;
    using base_t::free;
    using base_t::clear;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#include "nixycore/al/general/disable_warnings.hxx"
//////////////////////////////////////////////////////////////////////////
