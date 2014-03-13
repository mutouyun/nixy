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

template <typename Type_, typename Seq_ = nx::deque<Type_> >
class stack : public std::stack<Type_, Seq_>
{
public:
    typedef std::stack<Type_, Seq_> base_t;

public:
    explicit stack(const Seq_& s = Seq_())
        : base_t(s)
    {}

    stack(const stack& rhs)
        : base_t(rhs)
    {}

    stack(const rvalue<stack, true>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
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

template <typename T_, typename S_>
inline void swap(stack<T_, S_>& x, stack<T_, S_>& y)
{
    x.swap(y);
}

/*
    Special assign algorithm
*/

template <typename T_, typename S_, typename V>
inline void insert(stack<T_, S_>& set, typename stack<T_, S_>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T_, typename S_>
inline void erase(stack<T_, S_>& set, typename stack<T_, S_>::iterator /*ite*/)
{
    set.pop();
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
