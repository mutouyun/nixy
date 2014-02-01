/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "container/deque.h"

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "algorithm/algorithm.h"

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
};

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
