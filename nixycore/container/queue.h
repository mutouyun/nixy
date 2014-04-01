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

// std::queue
#include <queue>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, typename SeqT = nx::deque<T> >
class queue : public std::queue<T, SeqT>
{
public:
    typedef std::queue<T, SeqT> base_t;

public:
    explicit queue(const SeqT& s = SeqT())
        : base_t(s)
    {}

    queue(const queue& rhs)
        : base_t(rhs)
    {}

    queue(const rvalue<queue, true>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
    }

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

/*
    Special assign algorithm
*/

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

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
