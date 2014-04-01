/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typequalifier.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/pattern/iterator.h"
#include "nixycore/bugfix/assert.h"
#include "nixycore/algorithm/foreach.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_skip_array
{
    /*
        X raised by the exponent of Y
    */

    template <size_t X, size_t Y>
    struct exp
    {
        NX_STATIC_VALUE(size_t, X * exp<X, Y - 1>::value);
    };

    template <size_t X>
    struct exp<X, 1>
    {
        NX_STATIC_VALUE(size_t, X);
    };

    template <size_t X>
    struct exp<X, 0>
    {
        NX_STATIC_VALUE(size_t, 1);
    };

    /*
        elems operations
    */

    template <size_t SkipN, size_t LevelN, class AllocT>
    struct elems
    {
        nx_assert_static(LevelN);

        NX_STATIC_PROPERTY(size_t, DEEP_SIZE, exp<SkipN, LevelN - 1>::value);

        typedef pvoid type_t[SkipN];
        typedef elems<SkipN, LevelN - 1, AllocT> deep_elems;

        static size_t index(size_t i)
        {
            return i / DEEP_SIZE;
        }

        static size_t jump(size_t i, size_t n)
        {
            size_t jumped = n * DEEP_SIZE;
            nx_assert(i >= jumped);
            return (i - jumped);
        }

        static pvoid& at(type_t& e, size_t i)
        {
            size_t n = index(i);
            nx_assert(n < SkipN);
            pvoid& p = e[n];
            if (!p)
            {
                p = AllocT::alloc(sizeof(type_t));
                nx_assert(p);
                memset(p, 0, sizeof(type_t));
            }
            return deep_elems::at(*static_cast<type_t*>(p), jump(i, n));
        }

        static void clear(type_t& e)
        {
            for(size_t i = 0; i < SkipN; ++i)
            {
                type_t* p = static_cast<type_t*>(e[i]);
                if (!p) continue;
                deep_elems::clear(*p);
                AllocT::free(p);
            }
        }

        static size_t check(const type_t& e, size_t i)
        {
            size_t n, r = 0, x, j;
            do
            {
                n = index(i + r);
                nx_assert(n < SkipN);
                pvoid p = 0;
                for(; (n < SkipN) && !(p = e[n]); ++n)
                    r += DEEP_SIZE;
                j = jump(r + i, n);
                if (p)
                    x = deep_elems::check(*static_cast<const type_t*>(p), j);
                else
                    x = 0;
                r += x;
            } while((j + x) >= DEEP_SIZE);
            return r;
        }
    };

    template <size_t SkipN, class AllocT>
    struct elems<SkipN, 1, AllocT>
    {
        typedef pvoid type_t[SkipN];

        static pvoid& at(type_t& e, size_t i)
        {
            nx_assert(i < SkipN);
            return e[i];
        }

        static void clear(type_t& /*e*/)
        {
            // Do nothing
        }

        static size_t check(const type_t& e, size_t i)
        {
            nx_assert(i < SkipN);
            size_t r = 0;
            for(size_t n = i; (n < SkipN) && !e[n]; ++n)
                r += 1;
            return r;
        }
    };

    /*
        iterator policy
    */

    template <typename ElemsT, typename TypeT>
    struct iter_policy
    {
        typedef ElemsT elems_ops;
        typedef TypeT  type_t;

        typename elems_ops::type_t* elems_ptr_;
        size_t index_;

        iter_policy(void)
            : elems_ptr_(nx::nulptr)
            , index_(0)
        {}

        size_t index(void) const
        {
            return index_;
        }

        void relocate(bool is_plus = true)
        {
            nx_assert(elems_ptr_);
            if (is_plus)
                index_ += elems_ops::check(*elems_ptr_, index_);
            else
                index_ -= elems_ops::check(*elems_ptr_, index_);
        }

        void operator()(int n)
        {
            if (n == 0) return;
            nx_assert(elems_ptr_);
            int x = (n > 0) ? 1 : -1;
            nx_foreach(NX_UNUSED i, n)
            {
                index_ += x;
                relocate(n > 0);
            }
        }

        type_t value(void) const
        {
            nx_assert(elems_ptr_);
            typedef typename nx::cp_const<type_t, pvoid>::type_t pvoid_t;
            pvoid_t& p = const_cast<pvoid_t&>(elems_ops::at(*elems_ptr_, index_));
            return reinterpret_cast<type_t>(p);
        }

        bool operator==(const iter_policy& y) const
        {
            return (elems_ptr_ == y.elems_ptr_) &&
                   (index_     == y.index_);
        }
    };
}

template <typename T, size_t SkipN, size_t LevelN, class AllocT, bool = nx::is_pointer<T>::value>
class skip_array;

template <typename T, size_t SkipN, size_t LevelN, class AllocT>
class skip_array<T, SkipN, LevelN, AllocT, true>
{
    typedef private_skip_array::elems<SkipN, LevelN, AllocT> elems_ops;
    mutable typename elems_ops::type_t elems_;

public:
    NX_STATIC_PROPERTY(size_t, MAX, private_skip_array::exp<SkipN, LevelN>::value);

    typedef T                   value_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

private:
    typedef private_skip_array::iter_policy<elems_ops, reference>       iter_policy;
    typedef private_skip_array::iter_policy<elems_ops, const_reference> const_iter_policy;

public:
    typedef nx::iterator<iter_policy>       iterator;
    typedef nx::iterator<const_iter_policy> const_iterator;

public:
    skip_array(void)
    {
        memset(elems_, 0, sizeof(elems_));
    }

    ~skip_array(void)
    {
        elems_ops::clear(elems_);
    }

public:
    /*
        iterator support
    */

    iterator begin(void)
    {
        iterator iter;
        iter.elems_ptr_ = &elems_;
        iter.relocate();
        return iter;
    }

    const_iterator begin(void) const
    {
        const_iterator iter;
        iter.elems_ptr_ = &elems_;
        iter.relocate();
        return iter;
    }

    iterator end(void)
    {
        iterator iter;
        iter.elems_ptr_ = &elems_;
        iter.index_     = MAX;
        return iter;
    }

    const_iterator end(void) const
    {
        const_iterator iter;
        iter.elems_ptr_ = &elems_;
        iter.index_     = MAX;
        return iter;
    }

    // operator[]
    reference operator[](size_type i)
    {
        nx_assert(i < MAX);
        return reinterpret_cast<reference>(elems_ops::at(elems_, i));
    }
    const_reference operator[](size_type i) const
    {
        nx_assert(i < MAX);
        return reinterpret_cast<const_reference>(elems_ops::at(elems_, i));
    }

    // at()
    reference       at(size_type i)         { return operator[](i); }
    const_reference at(size_type i) const   { return operator[](i); }

    // front() and back()
    reference front()                       { return operator[](0); }
    const_reference front() const           { return operator[](0); }
    reference back()                        { return operator[](MAX - 1); }
    const_reference back() const            { return operator[](MAX - 1); }

    // size is constant
    static size_type size()                 { return MAX; }
    static bool empty()                     { return false; }
    static size_type max_size()             { return MAX; }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
