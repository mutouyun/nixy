/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/container/deque.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <stack> // std::stack

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, typename SeqT = nx::deque<T> >
using stack = std::stack<T, SeqT>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename T, typename SeqT = nx::deque<T> >
class stack : public std::stack<T, SeqT>
{
    typedef std::stack<T, SeqT> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::stack;
#else /*NX_SP_CXX11_INHERITING*/
    explicit stack(const SeqT& s = SeqT())
        : base_t(s)
    {}

    stack(const stack& rhs)
        : base_t(rhs)
    {}

#ifdef NX_SP_CXX11_STACK_SWAP
    stack(nx_rref(stack, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif
#endif/*NX_SP_CXX11_INHERITING*/

    stack& operator=(stack rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, typename S>
inline void swap(stack<T, S>& x, stack<T, S>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

/*
    Special assign algorithm
*/

template <typename T, typename S, typename V>
inline void insert(std::stack<T, S>& set, typename std::stack<T, S>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S>
inline void erase(std::stack<T, S>& set, typename std::stack<T, S>::iterator /*ite*/)
{
    set.pop();
}

#ifndef NX_SP_CXX11_ALIAS
template <typename T, typename S, typename V>
inline void insert(stack<T, S>& set, typename stack<T, S>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T, typename S>
inline void erase(stack<T, S>& set, typename stack<T, S>::iterator /*ite*/)
{
    set.pop();
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
