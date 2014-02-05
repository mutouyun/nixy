/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/fixedpool.h"
#include "memory/construct.h"

#include "delegate/functor.h"
#include "delegate/bind.h"

#include "bugfix/assert.h"

#include "general/general.h"
#include "preprocessor/preprocessor.h"
#include "typemanip/typemanip.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_object_pool
{
    template <typename T>
    T* construct(pvoid p)
    {
        return nx_construct(p, T);
    }

#define NX_OBJECT_POOL_HELPER_(n) \
    template <typename T, NX_PP_TYPE_1(n, typename P)> \
    T* construct(pvoid p, NX_PP_TYPE_2(n, P, par)) \
    { \
        return nx_construct(p, T, NX_PP_TYPE_1(n, par)); \
    }
    NX_PP_MULT_MAX(NX_OBJECT_POOL_HELPER_)
#undef NX_OBJECT_POOL_HELPER_
}

template <typename T, class FixedAlloc_ = fixed_pool<> >
class object_pool : NonCopyable
{
public:
    typedef T type_t;

private:
    FixedAlloc_ allocator_;

    functor<type_t*(pvoid)> constructor_;

    struct alloc_t
    {
        nx::byte data_[sizeof(type_t)];
        alloc_t* next_;
    };

    alloc_t* free_head_;
    alloc_t* free_tail_;

    size_t min_size_;
    size_t max_size_;
    size_t size_;

public:
    object_pool(size_t min_sz = 0, size_t max_sz = (size_t)~0)
        : allocator_(sizeof(alloc_t))
        , free_head_(nx::nulptr)
        , free_tail_(nx::nulptr)
        , min_size_(0)
        , max_size_(0)
        , size_(0)
    {
        constructor_ = static_cast<type_t*(*)(pvoid)>(&private_object_pool::construct<type_t>);
        limit(min_sz, max_sz);
    }

#define NX_OBJECT_POOL_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    object_pool(NX_PP_TYPE_2(n, P, par), size_t min_sz = 0, size_t max_sz = (size_t)~0) \
        : allocator_(sizeof(alloc_t)) \
        , free_head_(nx::nulptr) \
        , free_tail_(nx::nulptr) \
        , min_size_(0) \
        , max_size_(0) \
        , size_(0) \
    { \
        constructor_ = bind(&private_object_pool::construct<type_t, NX_PP_TYPE_1(n, P)>, \
                                                                _1, NX_PP_TYPE_1(n, par)); \
        limit(min_sz, max_sz); \
    }
    NX_PP_MULT_MAX(NX_OBJECT_POOL_)
#undef NX_OBJECT_POOL_

    ~object_pool()
    {
        while(free_head_) destroy();
    }

private:
    type_t* create(void)
    {
        alloc_t* p = static_cast<alloc_t*>(allocator_.alloc());
        nx_assert(p);
        p->next_ = nx::nulptr;
        return constructor_(p->data_);
    }

    void destroy(void)
    {
        nx_assert(free_head_);
        alloc_t* p = free_head_;
        free_head_ = free_head_->next_;
        -- size_;
        nx_destruct(reinterpret_cast<type_t*>(p), type_t);
        allocator_.free(p);
    }

    type_t* pick(void)
    {
        nx_assert(free_head_);
        alloc_t* p = free_head_;
        free_head_ = free_head_->next_;
        if (free_head_)
            p->next_ = nx::nulptr;
        else
            free_tail_ = free_head_;
        -- size_;
        return reinterpret_cast<type_t*>(p->data_);
    }

public:
    void limit(size_t min_sz = 0, size_t max_sz = (size_t)~0)
    {
        nx_assert(max_sz);
        nx_assert(min_sz <= max_sz);
        min_size_ = min_sz;
        max_size_ = max_sz;
        if (size_ > max_size_)
        {
            do
            {
                destroy();
            }
            while (size_ > max_size_);
        }
        else
        {
            while (size_ < min_size_)
            {
                free(create());
            }
        }
    }

    size_t min_size(void) const { return min_size_; }
    size_t max_size(void) const { return max_size_; }

    size_t size(void) const { return size_; }

public:
    type_t* alloc(void)
    {
        if(!free_head_) free(create());
        return pick();
    }

    void free(type_t* objc)
    {
        if (!objc) return;
        alloc_t* p = reinterpret_cast<alloc_t*>(objc);
        nx_assert(!(p->next_));
        if (p->next_) return;
        if (size_ < max_size_)
        {
            if (free_tail_)
                free_tail_->next_ = p;
            free_tail_ = p;
            if (!free_head_) free_head_ = free_tail_;
            ++ size_;
        }
        else
        {
            nx_destruct(objc, type_t);
            allocator_.free(p);
        }
    }

    void clear(void)
    {
        while(free_head_ && (size_ > min_size_)) destroy();
        if (!free_head_) free_tail_ = free_head_;
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
