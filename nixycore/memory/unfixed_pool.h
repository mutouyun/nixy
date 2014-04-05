/*
    The Nixy Library
    Code covered by the MIT License

    Modified from StdExt - C++ STL Extensions (http://code.google.com/p/stdext/)
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2004 - 2008 xushiwei (xushiweizh@gmail.com)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifndef NX_UNFIXEDPOOL_BLOCKSIZE
#define NX_UNFIXEDPOOL_BLOCKSIZE    (sizeof(nx::pvoid) * 1024)
#endif

/*
    Unfixed Memory Pool
*/

template
<
    class  AllocT     = NX_DEFAULT_ALLOC,
    size_t BlockSizeN = NX_UNFIXEDPOOL_BLOCKSIZE
>
class unfixed_pool : noncopyable
{
    struct header_t
    {
        pvoid  prev_;
        size_t size_;
    };

    NX_STATIC_PROPERTY(size_t, HEAD_SIZE, sizeof(header_t));
    NX_STATIC_PROPERTY(size_t, BODY_SIZE, BlockSizeN - HEAD_SIZE);

    struct block_t : header_t
    {
        nx::byte buff_[BODY_SIZE];
    };

    nx::byte * head_, * tail_;

    void init(void)
    {
        tail_ = head_ = reinterpret_cast<nx::byte*>(HEAD_SIZE);
    }

    block_t* blocks_head(void) const
    {
        return reinterpret_cast<block_t*>(head_ - HEAD_SIZE);
    }

public:
    unfixed_pool(void)
        : head_(0), tail_(0)
    { init(); }

    ~unfixed_pool(void)
    { clear(); }

public:
    size_t remain(void) const
    {
        return (tail_ - head_);
    }

    pvoid alloc(size_t size)
    {
        if (remain() < size)
        {
            if (size >= BlockSizeN)
            {
                block_t* new_blk = static_cast<block_t*>(nx::alloc<AllocT>(HEAD_SIZE + size));
                nx_assert(new_blk);
                new_blk->size_ = HEAD_SIZE + size;
                block_t* cur_blk = blocks_head();
                if (cur_blk)
                {
                    new_blk->prev_ = cur_blk->prev_;
                    cur_blk->prev_ = new_blk;
                }
                else
                {
                    new_blk->prev_ = 0;
                    tail_ = head_ = new_blk->buff_;
                }
                return new_blk->buff_;
            }
            else // size < BlockSizeN
            {
                block_t* new_blk = nx::alloc<AllocT, block_t>();
                nx_assert(new_blk);
                new_blk->size_ = sizeof(block_t);
                new_blk->prev_ = blocks_head();
                head_ = new_blk->buff_;
                tail_ = head_ + BlockSizeN;
            }
        }
        return (tail_ -= size);
    }

    void clear(void)
    {
        block_t* cur_blk = blocks_head();
        while(cur_blk)
        {
            block_t* tmp_blk = static_cast<block_t*>(cur_blk->prev_);
            nx::free<AllocT>(cur_blk, cur_blk->size_);
            cur_blk = tmp_blk;
        }
        init();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
