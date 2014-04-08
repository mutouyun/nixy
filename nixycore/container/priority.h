/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/container/vector.h"

#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

// std::priority_queue
#include <queue>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template
<
    typename T, typename SeqT = nx::vector<T>,
    typename CompT = std::less<typename SeqT::value_type>
>
class priority : public std::priority_queue<T, SeqT, CompT>
{
public:
    typedef std::priority_queue<T, SeqT, CompT> base_t;

public:
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

    priority(const rvalue<priority, true>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
    }

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

/*
    Special assign algorithm
*/

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

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
