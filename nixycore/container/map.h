/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <map> // std::map

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename KeyT, typename TypeT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
using map = std::map<KeyT, TypeT, CompT, 
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>;
template <typename KeyT, typename TypeT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
using multimap = std::multimap<KeyT, TypeT, CompT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename KeyT, typename TypeT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class map : public std::map<KeyT, TypeT, CompT, 
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>
{
    typedef std::map<KeyT, TypeT, CompT, 
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::map;
#else /*NX_SP_CXX11_INHERITING*/
    map(void)
        : base_t()
    {}

    explicit map(const typename base_t::allocator_type& a)
        : base_t(a)
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
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    map& operator=(map rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

template <typename KeyT, typename TypeT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class multimap : public std::multimap<KeyT, TypeT, CompT, 
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>
{
    typedef std::multimap<KeyT, TypeT, CompT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::multimap;
#else /*NX_SP_CXX11_INHERITING*/
    multimap(void)
        : base_t()
    {}

    explicit multimap(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit multimap(const CompT& c,
                      const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(c, a)
    {}

    template <typename IteratorT>
    multimap(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    multimap(IteratorT f, IteratorT l,
             const CompT& c,
             const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}

    multimap(const multimap& rhs)
        : base_t(rhs)
    {}

    multimap(nx_rref(multimap, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    multimap& operator=(multimap rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename K, typename T, typename C, class A>
inline void swap(map<K, T, C, A>& x, map<K, T, C, A>& y)
{
    x.swap(y);
}

template <typename K, typename T, typename C, class A>
inline void swap(multimap<K, T, C, A>& x, multimap<K, T, C, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

/*
    Special container traits
*/

template <typename K, typename T, class C, class A>
struct container_traits<std::map<K, T, C, A> >
{
    typedef typename std::map<K, T, C, A>::value_type val_t;
    typedef typename std::map<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

template <typename K, typename T, class C, class A>
struct container_traits<std::multimap<K, T, C, A> >
{
    typedef typename std::multimap<K, T, C, A>::value_type val_t;
    typedef typename std::multimap<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

#ifndef NX_SP_CXX11_ALIAS
template <typename K, typename T, class C, class A>
struct container_traits<nx::map<K, T, C, A> >
{
    typedef typename nx::map<K, T, C, A>::value_type val_t;
    typedef typename nx::map<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

template <typename K, typename T, class C, class A>
struct container_traits<nx::multimap<K, T, C, A> >
{
    typedef typename nx::multimap<K, T, C, A>::value_type val_t;
    typedef typename nx::multimap<K, T, C, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
