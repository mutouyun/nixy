/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/algorithm/algorithm.h"

// std::deque
#include <deque>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Type_, class Alloc_ = NX_DEFAULT_ALLOC>
class deque : public std::deque<Type_, typename Alloc_::template std_allocator<Type_>::type_t>
{
public:
    typedef std::deque<Type_, typename Alloc_::template std_allocator<Type_>::type_t> base_t;

public:
    deque(void)
        : base_t()
    {}
    deque(const base_t& x)
        : base_t(x)
    {}

    explicit deque(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit deque(typename base_t::size_type n,
                   const typename base_t::value_type& v = typename base_t::value_type(),
                   const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, v, a)
    {}

    template <typename Iterator_>
    deque(Iterator_ f, Iterator_ l,
          const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    deque(const deque& rhs)
        : base_t(rhs)
    {}
    deque(const rvalue<deque>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
    }

    deque& operator=(deque rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T_, class A_>
inline void swap(deque<T_, A_>& x, deque<T_, A_>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
