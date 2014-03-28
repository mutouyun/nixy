/*
    The Nixy Library
    Code covered by the MIT License

    Modified from StdExt - C++ STL Extensions (http://code.google.com/p/stdext/)
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2004 - 2008 xushiwei (xushiweizh@gmail.com)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifndef NX_UNFIXEDPOOL_BLOCKSIZE
#define NX_UNFIXEDPOOL_BLOCKSIZE \
    (nx::by_pool_stack_fixed<Alloc_>::LARGE_SIZ - \
     nx::by_pool_stack_fixed<Alloc_>::LARGE_INC)
#endif

/*
    Unfixed Memory Pool
*/

template
<
    class  Alloc_     = NX_DEFAULT_ALLOC,
    size_t BlockSize_ = NX_UNFIXEDPOOL_BLOCKSIZE
>
class unfixed_pool : noncopyable
{
    struct block_t
    {
        block_t* prev_;
        nx::byte buff_[BlockSize_];
    };

    NX_STATIC_PROPERTY(size_t, HEADER_SIZE, sizeof(block_t) - BlockSize_);

    nx::byte * head_, * tail_;

    void init(void)
    {
        tail_ = head_ = reinterpret_cast<nx::byte*>(HEADER_SIZE);
    }

    block_t* blocks_head(void) const
    {
        return reinterpret_cast<block_t*>(head_ - HEADER_SIZE);
    }

public:
    unfixed_pool(void)
        : head_(nx::nulptr), tail_(nx::nulptr)
    { init(); }

    ~unfixed_pool() { clear(); }

public:
    size_t remain(void) const
    {
        return (tail_ - head_);
    }

    pvoid alloc(size_t size)
    {
        if (remain() < size)
        {
            if (size >= BlockSize_)
            {
                block_t* new_blk = static_cast<block_t*>(Alloc_::alloc(HEADER_SIZE + size));
                nx_assert(new_blk);
                block_t* cur_blk = blocks_head();
                if (cur_blk)
                {
                    new_blk->prev_ = cur_blk->prev_;
                    cur_blk->prev_ = new_blk;
                }
                else
                {
                    new_blk->prev_ = nx::nulptr;
                    tail_ = head_ = new_blk->buff_;
                }
                return new_blk->buff_;
            }
            else // size < BlockSize_
            {
                block_t* new_blk = static_cast<block_t*>(Alloc_::alloc(sizeof(block_t)));
                nx_assert(new_blk);
                new_blk->prev_ = blocks_head();
                head_ = new_blk->buff_;
                tail_ = head_ + BlockSize_;
            }
        }
        return (tail_ -= size);
    }

    void clear(void)
    {
        block_t* cur_blk = blocks_head();
        while(cur_blk)
        {
            block_t* tmp_blk = cur_blk->prev_;
            Alloc_::free(cur_blk);
            cur_blk = tmp_blk;
        }
        init();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
