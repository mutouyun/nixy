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

template <typename KeyT, typename CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class set : public std::set<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t>
{
public:
    typedef std::set<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t> base_t;

public:
    set(void)
        : base_t()
    {}

    explicit set(const CompT& c,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
            : base_t(c, a)
    {}

    template <typename Iterator_>
    set(Iterator_ f, Iterator_ l)
        : base_t(f, l)
    {}

    template <typename Iterator_>
    set(Iterator_ f, Iterator_ l,
        const CompT& c,
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

template <typename K, typename C, class A>
inline void swap(set<K, C, A>& x, set<K, C, A>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
