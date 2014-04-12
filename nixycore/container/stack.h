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

// std::stack
#include <stack>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, typename SeqT = nx::deque<T> >
class stack : public std::stack<T, SeqT>
{
public:
    typedef std::stack<T, SeqT> base_t;

public:
    explicit stack(const SeqT& s = SeqT())
        : base_t(s)
    {}

    stack(const stack& rhs)
        : base_t(rhs)
    {}

    stack(nx_rref(stack, true) rhs)
        : base_t()
    {
        base_t::swap(moved(rhs));
    }

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

/*
    Special assign algorithm
*/

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

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
