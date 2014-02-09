/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

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
    deque()
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
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
