/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/std_alloc.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/pattern/iterator.h"
#include "nixycore/algorithm/series.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Memory Expand Models
*/

template
<
    class Alloc_, size_t IterCount_,
    template <typename, size_t> class Model_
>
struct by_pool_expand_keep // Never return memory back to system
{
    nx_assert_static(IterCount_);

private:
    nx::iterator<Model_<size_t, IterCount_> > count_ite_;

protected:
    size_t count(void) const
    {
        return (*count_ite_);
    }

    pvoid expand(size_t block_size)
    {
        nx_assert(block_size);
        return Alloc_::alloc( block_size * (*(++count_ite_)) );
    }
};

template
<
    class Alloc_, size_t IterCount_,
    template <typename, size_t> class Model_
>
struct by_pool_expand_return : by_pool_expand_keep<Alloc_, IterCount_, Model_>
{
private:
    typedef by_pool_expand_keep<Alloc_, IterCount_, Model_> base_t;

    struct blocks_t
    {
        pvoid     data_;
        blocks_t* next_;
    } * blocks_head_;

protected:
    by_pool_expand_return(void)
        : blocks_head_(nx::nulptr)
    {}

    ~by_pool_expand_return()
    {
        while(blocks_head_)
        {
            blocks_t* blocks = blocks_head_;
            blocks_head_ = blocks_head_->next_;
            Alloc_::free(blocks->data_);
            Alloc_::free(blocks);
        }
    }

    pvoid expand(size_t block_size)
    {
        blocks_t* blocks = (blocks_t*)Alloc_::alloc(sizeof(blocks_t));
        blocks->data_ = base_t::expand(block_size);
        blocks->next_ = blocks_head_;
        blocks_head_ = blocks;
        return blocks->data_;
    }
};

/*
    Default allocation parameters
*/

#ifndef NX_FIXEDPOOL_MODEL
#define NX_FIXEDPOOL_MODEL      nx::by_iter_powerof
#endif

#ifndef NX_FIXEDPOOL_ITERCOUNT
#define NX_FIXEDPOOL_ITERCOUNT  2
#endif

/*
    Fixed Memory Pool
*/

template
<
    class Alloc_ = by_alloc_std,

    template
    <
        class, size_t,
        template <typename, size_t> class
    >
    class Expand_ = by_pool_expand_return,      /* Memory growth model uses the by_pool_expand_return */

    template <typename, size_t>
    class Model_ = NX_FIXEDPOOL_MODEL,          /* Iterative algorithm using NX_FIXEDPOOL_MODEL */

    size_t IterCount_ = NX_FIXEDPOOL_ITERCOUNT  /* Iteration count */
>
class fixed_pool : Expand_<Alloc_, IterCount_, Model_>, noncopyable
{
public:
    typedef Expand_<Alloc_, IterCount_, Model_> base_t;

private:
    size_t block_size_;
    pvoid cursor_;

    void expand(void)
    {
        pvoid* p = (pvoid*)(cursor_ = base_t::expand(block_size_));
        nx_assert(p);
        for(size_t i = 0; i < base_t::count() - 1; ++i)
            p = (pvoid*)( (*p) = ((byte*)p) + block_size() );
        (*p) = nx::nulptr;
    }

public:
    explicit fixed_pool(size_t size = 0)
        : block_size_(size)
        , cursor_(nx::nulptr)
    {}

    void init_block_size(size_t block_size)
    {
        nx_assert(block_size_ == 0);
        block_size_ = block_size;
        nx_assert(block_size_ > sizeof(pvoid));
    }

    size_t block_size(void) const { return block_size_; }

public:
    pvoid alloc(void)
    {
        if (!cursor_) expand();
        pvoid p = cursor_;
        cursor_ = *(pvoid*)p;   // Get next block
        return p;
    }

    void free(pvoid p)
    {
        if (!p) return;
        *(pvoid*)p = cursor_;
        cursor_ = p;            // Save free block
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
