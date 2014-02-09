/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/container/deque.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/algorithm/algorithm.h"

// std::queue
#include <queue>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Type_, typename Seq_ = nx::deque<Type_> >
class queue : public std::queue<Type_, Seq_>
{
public:
    typedef std::queue<Type_, Seq_> base_t;

public:
    explicit queue(const Seq_& s = Seq_())
        : base_t(s)
    {}
};

/*
    Special assign algorithm
*/

template <typename T_, typename S_, typename V>
inline void insert(queue<T_, S_>& set, typename queue<T_, S_>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T_, typename S_>
inline void erase(queue<T_, S_>& set, typename queue<T_, S_>::iterator /*ite*/)
{
    set.pop();
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
