/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/stdalloc.h"

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
struct by_pool_expand_keep
{
    nx_assert_static(IterCount_);

private:
    size_t block_size_;
    nx::iterator<Model_<size_t, IterCount_> > count_ite_;

public:
    explicit by_pool_expand_keep(size_t block_size)
        : block_size_(block_size)
    {}
    /* Never return memory back to system */

    void init_block_size(size_t block_size)
    {
        nx_assert(block_size_ == 0);
        nx_verify(block_size_ = block_size);
    }

    size_t block_size(void) const { return block_size_; }

protected:
    pvoid operator()(size_t* count_ptr)
    {
        nx_assert(block_size_ >= sizeof(pvoid));
        return Alloc_::alloc( block_size_ * ( (*count_ptr) = (*(++count_ite_)) ) );
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
        pvoid     block_;
        blocks_t* pnext_;
    } * phead_, * ptail_;

public:
    explicit by_pool_expand_return(size_t block_size)
        : base_t(block_size)
        , phead_(nx::nulptr)
        , ptail_(nx::nulptr)
    {}

    ~by_pool_expand_return()
    {
        while(phead_)
        {
            blocks_t* block = phead_;
            phead_ = phead_->pnext_;
            Alloc_::free(block->block_);
            Alloc_::free(block);
        }
    }

protected:
    pvoid operator()(size_t* count_ptr)
    {
        blocks_t* block = (blocks_t*)Alloc_::alloc(sizeof(blocks_t));
        block->block_ = base_t::operator()(count_ptr);
        block->pnext_ = nx::nulptr;
        if (ptail_) ptail_->pnext_ = block;
        ptail_ = block;
        if(!phead_) phead_ = ptail_;
        return (block->block_);
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
class fixed_pool : public Expand_<Alloc_, IterCount_, Model_>, noncopyable
{
public:
    typedef Expand_<Alloc_, IterCount_, Model_> base_t;

private:
    pvoid cursor_;

    void expand(void)
    {
        size_t count = 0;
        pvoid* p = (pvoid*)(cursor_ = (*this)(&count));
        for(size_t i = 1; i < count; ++i)
            p = (pvoid*)( (*p) = ((byte*)p) + base_t::block_size() );
        (*p) = nx::nulptr;
    }

public:
    explicit fixed_pool(size_t size = 0)
        : base_t(size)
        , cursor_(nx::nulptr)
    {}

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
        *(pvoid*)p = cursor_;
        cursor_ = p;            // Save free block
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
