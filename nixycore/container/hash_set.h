/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_UNORDERED_SET

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include <unordered_set> // std::unordered_set

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename KeyT, class HashT = std::hash<KeyT>
                       , class PredT = std::equal_to<KeyT>
                       , class AllocT = NX_DEFAULT_ALLOC>
using hash_set = std::unordered_set<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t>;
template <typename KeyT, class HashT = std::hash<KeyT>
                       , class PredT = std::equal_to<KeyT>
                       , class AllocT = NX_DEFAULT_ALLOC>
using hash_multiset = std::unordered_multiset<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename KeyT, class HashT = std::hash<KeyT>
                       , class PredT = std::equal_to<KeyT>
                       , class AllocT = NX_DEFAULT_ALLOC>
class hash_set : public std::unordered_set<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t>
{
    typedef std::unordered_set<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::unordered_set;
#else /*NX_SP_CXX11_INHERITING*/
    hash_set(void)
        : base_t()
    {}

    explicit hash_set(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit hash_set(typename base_t::size_type n,
                      const typename base_t::hasher& hf = typename base_t::hasher(),
                      const typename base_t::key_equal& eql = typename base_t::key_equal(),
                      const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, hf, eql, a)
    {}

    template <typename IteratorT>
    hash_set(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    hash_set(IteratorT f, IteratorT l,
             typename base_t::size_type n,
             const typename base_t::hasher& hf = typename base_t::hasher(),
             const typename base_t::key_equal& eql = typename base_t::key_equal(),
             const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, n, hf, eql, a)
    {}

    hash_set(const hash_set& rhs)
        : base_t(rhs)
    {}

    hash_set(nx_rref(hash_set, true) rhs)
        : base_t()
    {
        base_t::swap(moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    hash_set& operator=(hash_set rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

template <typename KeyT, class HashT = std::hash<KeyT>
                       , class PredT = std::equal_to<KeyT>
                       , class AllocT = NX_DEFAULT_ALLOC>
class hash_multiset : public std::unordered_multiset<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t>
{
    typedef std::unordered_multiset<KeyT, HashT, PredT, 
            typename AllocT::template std_allocator<KeyT>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::unordered_multiset;
#else /*NX_SP_CXX11_INHERITING*/
    hash_multiset(void)
        : base_t()
    {}

    explicit hash_multiset(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit hash_multiset(typename base_t::size_type n,
                           const typename base_t::hasher& hf = typename base_t::hasher(),
                           const typename base_t::key_equal& eql = typename base_t::key_equal(),
                           const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, hf, eql, a)
    {}

    template <typename IteratorT>
    hash_multiset(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    hash_multiset(IteratorT f, IteratorT l,
                  typename base_t::size_type n,
                  const typename base_t::hasher& hf = typename base_t::hasher(),
                  const typename base_t::key_equal& eql = typename base_t::key_equal(),
                  const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, n, hf, eql, a)
    {}

    hash_multiset(const hash_multiset& rhs)
        : base_t(rhs)
    {}

    hash_multiset(nx_rref(hash_multiset, true) rhs)
        : base_t()
    {
        base_t::swap(moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    hash_multiset& operator=(hash_multiset rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename K, class H, class P, class A>
inline void swap(hash_set<K, H, P, A>& x, hash_set<K, H, P, A>& y)
{
    x.swap(y);
}

template <typename K, class H, class P, class A>
inline void swap(hash_multiset<K, H, P, A>& x, hash_multiset<K, H, P, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#endif/*NX_SP_CXX11_UNORDERED_SET*/
