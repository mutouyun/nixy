/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/algorithm/algorithm.h"

// std::map
#include <map>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Key_, typename Type_, typename Comp_ = std::less<Key_>, class Alloc_ = NX_DEFAULT_ALLOC>
class map : public std::map<Key_, Type_, Comp_, typename Alloc_::template std_allocator<std::pair<const Key_, Type_> >::type_t>
{
public:
    typedef std::map<Key_, Type_, Comp_, typename Alloc_::template std_allocator<std::pair<const Key_, Type_> >::type_t> base_t;

public:
    map()
        : base_t()
    {}
    map(const base_t& x)
        : base_t(x)
    {}

    explicit map(const Comp_& c,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(c, a)
    {}

    template <typename Iterator_>
    map(Iterator_ f, Iterator_ l)
        : base_t(f, l)
    {}

    template <typename Iterator_>
    map(Iterator_ f, Iterator_ l,
        const Comp_& c,
        const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}
};

/*
    Special container traits
*/

template <typename K_, typename T_, typename C_, class A_>
struct container_traits<std::map<K_, T_, C_, A_> >
{
    typedef typename std::map<K_, T_, C_, A_>::value_type val_t;
    typedef typename std::map<K_, T_, C_, A_>::iterator   ite_t;
    typedef std::pair<K_, T_>                             pair_t;
};

template <typename K_, typename T_, typename C_, class A_>
struct container_traits<nx::map<K_, T_, C_, A_> >
{
    typedef typename nx::map<K_, T_, C_, A_>::value_type val_t;
    typedef typename nx::map<K_, T_, C_, A_>::iterator   ite_t;
    typedef std::pair<K_, T_>                            pair_t;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
