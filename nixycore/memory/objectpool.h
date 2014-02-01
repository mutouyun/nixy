/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/fixedpool.h"
#include "memory/construct.h"

#include "bugfix/assert.h"

#include "general/general.h"
#include "typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_object_pool
{
    template <typename T, size_t MinSize_, size_t MaxSize_, class FixedAlloc_>
    class detail
    {
    public:
        typedef T type_t;

    private:
        FixedAlloc_ allocator_;

        struct alloc_t
        {
            nx::byte data_[sizeof(type_t)];
            alloc_t* next_;
        };

        alloc_t* free_head_;
        alloc_t* free_tail_;
        
        size_t size_;

    public:
        detail(void)
            : allocator_(sizeof(alloc_t))
            , free_head_(nx::nulptr)
            , free_tail_(nx::nulptr)
            , size_(0)
        {}

        ~detail()
        {
            while(free_head_)
            {
                alloc_t* p = free_head_;
                free_head_ = free_head_->next_;
                nx_destruct(reinterpret_cast<type_t*>(p), type_t);
                allocator_.free(p);
            }
        }

    private:
        pvoid create(void)
        {
            alloc_t* p = static_cast<alloc_t*>(allocator_.alloc());
            nx_assert(p);
            p->next_ = nx::nulptr;
            return p->data_;
        }

    public:
        void make(void)
        {
            free(nx_construct(create(), type_t));
        }

#   define NX_OBJECT_POOL_MAKE_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        void make(NX_PP_TYPE_2(n, P, par)) \
        { \
            free(nx_construct(create(), type_t, NX_PP_TYPE_1(n, par))); \
        }
        NX_PP_MULT_MAX(NX_OBJECT_POOL_MAKE_)
#   undef NX_OBJECT_POOL_MAKE_

        type_t* alloc(void)
        {
            if (!free_head_) make();
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

        void free(type_t* objc)
        {
            if (!objc) return;
            alloc_t* p = reinterpret_cast<alloc_t*>(objc);
            nx_assert(!(p->next_));
            if (p->next_) return;
            if (size_ < MaxSize_)
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
            while(free_head_ && (size_ > MinSize_))
            {
                alloc_t* p = free_head_;
                free_head_ = free_head_->next_;
                -- size_;
                nx_destruct(reinterpret_cast<type_t*>(p), type_t);
                allocator_.free(p);
            }
            if (!free_head_) free_tail_ = free_head_;
        }

        size_t size(void) const { return size_; }
    };
}

template
<
    typename T, 
    size_t MinSize_ = 0, 
    size_t MaxSize_ = (size_t)~0, 
    class FixedAlloc_ = fixed_pool<>
>
class object_pool
    : public private_object_pool::detail<T, MinSize_, MaxSize_, FixedAlloc_>
{
    nx_assert_static(MaxSize_);
    nx_assert_static(MinSize_ <= MaxSize_);

    typedef private_object_pool::detail<T, MinSize_, MaxSize_, FixedAlloc_> base_t;

public:
    object_pool(void)
    { for(size_t i = 0; i < MinSize_; ++i) base_t::make(); }

#define NX_OBJECT_POOL_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    object_pool(NX_PP_TYPE_2(n, P, par)) \
    { for(size_t i = 0; i < MinSize_; ++i) base_t::make(NX_PP_TYPE_1(n, par)); }
    NX_PP_MULT_MAX(NX_OBJECT_POOL_)
#undef NX_OBJECT_POOL_
};

template <typename T, size_t MaxSize_, class FixedAlloc_>
class object_pool<T, 0, MaxSize_, FixedAlloc_>
    : public private_object_pool::detail<T, 0, MaxSize_, FixedAlloc_>
{
    nx_assert_static(MaxSize_);
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
