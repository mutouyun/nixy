/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

// std::map
#include <map>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename KeyT, typename TypeT, typename CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class map : public std::map<KeyT, TypeT, CompT, typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>
{
public:
    typedef std::map<KeyT, TypeT, CompT, typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t> base_t;

public:
    map(void)
        : base_t()
    {}

    explicit map(const CompT& c,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(c, a)
    {}

    template <typename IteratorT>
    map(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    map(IteratorT f, IteratorT l,
        const CompT& c,
        const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}

    map(const map& rhs)
        : base_t(rhs)
    {}

    map(nx_rref(map, true) rhs)
        : base_t()
    {
        base_t::swap(moved(rhs));
    }

    map& operator=(map rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special container traits
*/

template <typename K, typename T, typename C, class A>
struct container_traits<std::map<K, T, C, A> >
{
    typedef typename std::map<K, T, C, A>::value_type val_t;
    typedef typename std::map<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T>                             pair_t;
};

template <typename K, typename T, typename C, class A>
struct container_traits<nx::map<K, T, C, A> >
{
    typedef typename nx::map<K, T, C, A>::value_type val_t;
    typedef typename nx::map<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T>                            pair_t;
};

/*
    Special swap algorithm
*/

template <typename K, typename T, typename C, class A>
inline void swap(map<K, T, C, A>& x, map<K, T, C, A>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
