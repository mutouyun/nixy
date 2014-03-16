/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename P>
class stream_base : public P
{
public:
    typedef P policy_t;

public:
    template <typename T>
    friend const stream_base<P>& operator<<(const stream_base<P>& s, const T& data)
    {
        const_cast<stream_base<P>&>(s).policy_t::operator<<(data);
        return s;
    }

    template <typename T>
    friend const stream_base<P>& operator>>(const stream_base<P>& s, T& data)
    {
        const_cast<stream_base<P>&>(s).policy_t::operator>>(data);
        return s;
    }

    template <typename T>
    friend const stream_base<P>& operator, (const stream_base<P>& s, const T& data)
    {
        return s << data;
    }

    friend const stream_base<P>& operator<<(const stream_base<P>& s, stream_base<P>& (*opt)(stream_base<P>&))
    {
        return opt(const_cast<stream_base<P>&>(s));
    }

    friend const stream_base<P>& operator, (const stream_base<P>& s, stream_base<P>& (*opt)(stream_base<P>&))
    {
        return opt(const_cast<stream_base<P>&>(s));
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
