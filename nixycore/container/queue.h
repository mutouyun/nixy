/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/container/deque.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

#include <queue> // std::queue

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, typename SeqT = nx::deque<T> >
using queue = std::queue<T, SeqT>;
#else/*NX_SP_CXX11_ALIAS*/
template <typename T, typename SeqT = nx::deque<T> >
class queue : public std::queue<T, SeqT>
{
    typedef std::queue<T, SeqT> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::queue;
#else/*NX_SP_CXX11_INHERITING*/
    explicit queue(const SeqT& s = SeqT())
        : base_t(s)
    {}

    queue(const queue& rhs)
        : base_t(rhs)
    {}

#ifdef NX_SP_CXX11_STACK_SWAP
    queue(nx_rref(queue, true) rhs)
        : base_t()
    {
        base_t::swap(moved(rhs));
    }
#endif
#endif/*NX_SP_CXX11_INHERITING*/

    queue& operator=(queue rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, typename S>
inline void swap(queue<T, S>& x, queue<T, S>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

/*
    Special assign algorithm
*/

template <typename T, typename S, typename V>
inline void insert(std::queue<T, S>& set, typename std::queue<T, S>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S>
inline void erase(std::queue<T, S>& set, typename std::queue<T, S>::iterator /*ite*/)
{
    set.pop();
}

#ifndef NX_SP_CXX11_ALIAS
template <typename T, typename S, typename V>
inline void insert(queue<T, S>& set, typename queue<T, S>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S>
inline void erase(queue<T, S>& set, typename queue<T, S>::iterator /*ite*/)
{
    set.pop();
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
