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
class stream_wrap : public P
{
public:
    typedef P policy_t;

public:
    template <typename T>
    friend const stream_wrap<P>& operator<<(const stream_wrap<P>& s, const T& data)
    {
        const_cast<stream_wrap<P>&>(s).policy_t::operator<<(data);
        return s;
    }

    template <typename T>
    friend const stream_wrap<P>& operator>>(const stream_wrap<P>& s, T& data)
    {
        const_cast<stream_wrap<P>&>(s).policy_t::operator>>(data);
        return s;
    }

    template <typename T>
    friend const stream_wrap<P>& operator, (const stream_wrap<P>& s, const T& data)
    {
        return s << data;
    }

    friend const stream_wrap<P>& operator<<(const stream_wrap<P>& s, stream_wrap<P>& (*opt)(stream_wrap<P>&))
    {
        return opt(const_cast<stream_wrap<P>&>(s));
    }

    friend const stream_wrap<P>& operator, (const stream_wrap<P>& s, stream_wrap<P>& (*opt)(stream_wrap<P>&))
    {
        return opt(const_cast<stream_wrap<P>&>(s));
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
