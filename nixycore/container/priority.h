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
    typename Type_, typename Seq_ = nx::vector<Type_>,
    typename Comp_ = std::less<typename Seq_::value_type>
>
class priority : public std::priority_queue<Type_, Seq_, Comp_>
{
public:
    typedef std::priority_queue<Type_, Seq_, Comp_> base_t;

public:
    explicit priority(const Comp_& c = Comp_(), const Seq_& s = Seq_())
        : base_t(c, s)
    {}

    template <typename Iterator_>
    priority(Iterator_ f, Iterator_ l,
             const Comp_& c = Comp_(), const Seq_& s = Seq_())
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

template <typename T_, typename S_, typename C_>
inline void swap(priority<T_, S_, C_>& x, priority<T_, S_, C_>& y)
{
    x.swap(y);
}

/*
    Special assign algorithm
*/

template <typename T_, typename S_, typename C_, typename V>
inline void insert(priority<T_, S_, C_>& set, typename priority<T_, S_, C_>::iterator /*ite*/, const V& val)
{
    set.push(val);
}

template <typename T_, typename S_, typename C_>
inline void erase(priority<T_, S_, C_>& set, typename priority<T_, S_, C_>::iterator /*ite*/)
{
    set.pop();
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
