/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <set> // std::set

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename KeyT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
using set = std::set<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t>;
template <typename KeyT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
using multiset = std::multiset<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename KeyT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class set : public std::set<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t>
{
    typedef std::set<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::set;
#else /*NX_SP_CXX11_INHERITING*/
    set(void)
        : base_t()
    {}

    explicit set(const typename base_t::allocator_type& a)
            : base_t(a)
    {}

    explicit set(const CompT& c,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
            : base_t(c, a)
    {}

    template <typename IteratorT>
    set(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    set(IteratorT f, IteratorT l,
        const CompT& c,
        const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}

    set(const set& rhs)
        : base_t(rhs)
    {}

    set(nx_rref(set, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    set& operator=(set rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

template <typename KeyT, class CompT = std::less<KeyT>, class AllocT = NX_DEFAULT_ALLOC>
class multiset : public std::multiset<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t>
{
    typedef std::multiset<KeyT, CompT, typename AllocT::template std_allocator<KeyT>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::multiset;
#else /*NX_SP_CXX11_INHERITING*/
    multiset(void)
        : base_t()
    {}

    explicit multiset(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit multiset(const CompT& c,
                      const typename base_t::allocator_type& a = typename base_t::allocator_type())
            : base_t(c, a)
    {}

    template <typename IteratorT>
    multiset(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    template <typename IteratorT>
    multiset(IteratorT f, IteratorT l,
             const CompT& c,
             const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, c, a)
    {}

    multiset(const multiset& rhs)
        : base_t(rhs)
    {}

    multiset(nx_rref(multiset, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    multiset& operator=(multiset rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename K, class C, class A>
inline void swap(set<K, C, A>& x, set<K, C, A>& y)
{
    x.swap(y);
}

template <typename K, class C, class A>
inline void swap(multiset<K, C, A>& x, multiset<K, C, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
