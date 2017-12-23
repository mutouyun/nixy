/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_UNORDERED_MAP

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include <unordered_map> // std::unordered_map

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename KeyT, typename TypeT, class HashT = std::hash<KeyT>
                                       , class PredT = std::equal_to<KeyT>
                                       , class AllocT = NX_DEFAULT_ALLOC>
using hash_map = std::unordered_map<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>;
template <typename KeyT, typename TypeT, class HashT = std::hash<KeyT>
                                       , class PredT = std::equal_to<KeyT>
                                       , class AllocT = NX_DEFAULT_ALLOC>
using hash_multimap = std::unordered_multimap<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename KeyT, typename TypeT, class HashT = std::hash<KeyT>
                                       , class PredT = std::equal_to<KeyT>
                                       , class AllocT = NX_DEFAULT_ALLOC>
class hash_map : public std::unordered_map<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>
{
    typedef std::unordered_map<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::unordered_map;
#else /*NX_SP_CXX11_INHERITING*/
    hash_map(void)
        : base_t()
    {}

    explicit hash_map(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit hash_map(typename base_t::size_type n,
                      const typename base_t::hasher& hf = typename base_t::hasher(),
                      const typename base_t::key_equal& eql = typename base_t::key_equal(),
                      const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, hf, eql, a)
    {}

    template <typename IteratorT>
    hash_map(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    hash_map(IteratorT f, IteratorT l,
             typename base_t::size_type n,
             const typename base_t::hasher& hf = typename base_t::hasher(),
             const typename base_t::key_equal& eql = typename base_t::key_equal(),
             const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, n, hf, eql, a)
    {}

    hash_map(const hash_map& rhs)
        : base_t(rhs)
    {}

    hash_map(nx_rref(hash_map, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    hash_map& operator=(hash_map rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

template <typename KeyT, typename TypeT, class HashT = std::hash<KeyT>
                                       , class PredT = std::equal_to<KeyT>
                                       , class AllocT = NX_DEFAULT_ALLOC>
class hash_multimap : public std::unordered_multimap<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t>
{
    typedef std::unordered_multimap<KeyT, TypeT, HashT, PredT,
            typename AllocT::template std_allocator<std::pair<const KeyT, TypeT> >::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::unordered_multimap;
#else /*NX_SP_CXX11_INHERITING*/
    hash_multimap(void)
        : base_t()
    {}

    explicit hash_multimap(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit hash_multimap(typename base_t::size_type n,
                           const typename base_t::hasher& hf = typename base_t::hasher(),
                           const typename base_t::key_equal& eql = typename base_t::key_equal(),
                           const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, hf, eql, a)
    {}

    template <typename IteratorT>
    hash_multimap(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    hash_multimap(IteratorT f, IteratorT l,
                  typename base_t::size_type n,
                  const typename base_t::hasher& hf = typename base_t::hasher(),
                  const typename base_t::key_equal& eql = typename base_t::key_equal(),
                  const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, n, hf, eql, a)
    {}

    hash_multimap(const hash_multimap& rhs)
        : base_t(rhs)
    {}

    hash_multimap(nx_rref(hash_multimap, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    hash_multimap& operator=(hash_multimap rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename K, typename T, class H, class P, class A>
inline void swap(hash_map<K, T, H, P, A>& x, hash_map<K, T, H, P, A>& y)
{
    x.swap(y);
}

template <typename K, typename T, class H, class P, class A>
inline void swap(hash_multimap<K, T, H, P, A>& x, hash_multimap<K, T, H, P, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

/*
    Special container traits
*/

template <typename K, typename T, class H, class P, class A>
struct container_traits<std::unordered_map<K, T, H, P, A> >
{
    typedef typename std::unordered_map<K, T, H, P, A>::value_type val_t;
    typedef typename std::unordered_map<K, T, H, P, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

template <typename K, typename T, class H, class P, class A>
struct container_traits<std::unordered_multimap<K, T, H, P, A> >
{
    typedef typename std::unordered_multimap<K, T, H, P, A>::value_type val_t;
    typedef typename std::unordered_multimap<K, T, H, P, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

#ifndef NX_SP_CXX11_ALIAS
template <typename K, typename T, class H, class P, class A>
struct container_traits<nx::hash_map<K, T, H, P, A> >
{
    typedef typename nx::hash_map<K, T, H, P, A>::value_type val_t;
    typedef typename nx::hash_map<K, T, H, P, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};

template <typename K, typename T, class H, class P, class A>
struct container_traits<nx::hash_multimap<K, T, H, P, A> >
{
    typedef typename nx::hash_multimap<K, T, H, P, A>::value_type val_t;
    typedef typename nx::hash_multimap<K, T, H, P, A>::iterator   ite_t;
    typedef std::pair<K, T> pair_t;
};
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#endif/*NX_SP_CXX11_UNORDERED_MAP*/
