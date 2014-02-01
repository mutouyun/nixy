/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/alloc.h"

#include "general/general.h"
#include "typemanip/typemanip.h"

// std::set
#include <set>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Key_, typename Comp_ = std::less<Key_>, class Alloc_ = NX_DEFAULT_ALLOC>
class set : public std::set<Key_, Comp_, typename Alloc_::template std_allocator<Key_>::type_t>
{
public:
    typedef std::set<Key_, Comp_, typename Alloc_::template std_allocator<Key_>::type_t> base_t;

public:
    set()
        : base_t()
    {}
    set(const base_t& x)
        : base_t(x)
    {}

    explicit set(const Comp_& c,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
            : base_t(c, a)
    {}

    template <typename Iterator_>
    set(Iterator_ f, Iterator_ l)
        : base_t(f, l)
    {}

    template <typename Iterator_>
    set(Iterator_ f, Iterator_ l,
        const Comp_& c,
        const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
