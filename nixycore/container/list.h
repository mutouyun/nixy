/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

// std::list
#include <list>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Type_, class Alloc_ = NX_DEFAULT_ALLOC>
class list : public std::list<Type_, typename Alloc_::template std_allocator<Type_>::type_t>
{
public:
    typedef std::list<Type_, typename Alloc_::template std_allocator<Type_>::type_t> base_t;

public:
    list()
        : base_t()
    {}
    list(const base_t& x)
        : base_t(x)
    {}

    explicit list(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit list(typename base_t::size_type n,
                  const typename base_t::value_type& v = typename base_t::value_type(),
                  const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, v, a)
    {}

    template <typename Iterator_>
    list(Iterator_ f, Iterator_ l,
         const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
