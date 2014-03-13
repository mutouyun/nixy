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
    set(void)
        : base_t()
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

    set(const set& rhs)
        : base_t(rhs)
    {}

    set(const rvalue<set, true>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
    }

    set& operator=(set rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename K_, typename C_, class A_>
inline void swap(set<K_, C_, A_>& x, set<K_, C_, A_>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
