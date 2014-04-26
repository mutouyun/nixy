/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/container/vector.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <queue> // std::priority_queue

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template
<
    typename T, typename SeqT = nx::vector<T>,
    typename CompT = std::less<typename SeqT::value_type>
>
using priority = std::priority_queue<T, SeqT, CompT>;
#else /*NX_SP_CXX11_ALIAS*/
template
<
    typename T, typename SeqT = nx::vector<T>,
    typename CompT = std::less<typename SeqT::value_type>
>
class priority : public std::priority_queue<T, SeqT, CompT>
{
    typedef std::priority_queue<T, SeqT, CompT> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::priority_queue;
#else /*NX_SP_CXX11_INHERITING*/
    explicit priority(const CompT& c = CompT(), const SeqT& s = SeqT())
        : base_t(c, s)
    {}

    template <typename IteratorT>
    priority(IteratorT f, IteratorT l,
             const CompT& c = CompT(), const SeqT& s = SeqT())
        : base_t(f, l, c, s)
    {}

    priority(const priority& rhs)
        : base_t(rhs)
    {}

#ifdef NX_SP_CXX11_STACK_SWAP
    priority(nx_rref(priority, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif
#endif/*NX_SP_CXX11_INHERITING*/

    priority& operator=(priority rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, typename S, typename C>
inline void swap(priority<T, S, C>& x, priority<T, S, C>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

/*
    Special assign algorithm
*/

template <typename T, typename S, typename C, typename V>
inline void insert(std::priority_queue<T, S, C>& set, 
                   typename std::priority_queue<T, S, C>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S, typename C>
inline void erase(std::priority_queue<T, S, C>& set, 
                  typename std::priority_queue<T, S, C>::iterator /*ite*/)
{
    set.pop();
}

#ifndef NX_SP_CXX11_ALIAS
template <typename T, typename S, typename C, typename V>
inline void insert(priority<T, S, C>& set, typename priority<T, S, C>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S, typename C>
inline void erase(priority<T, S, C>& set, typename priority<T, S, C>::iterator /*ite*/)
{
    set.pop();
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
