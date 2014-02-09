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
class Stream : public P
{
public:
    typedef P policy_t;

public:
    template <typename T>
    friend const Stream<P>& operator<<(const Stream<P>& s, const T& data)
    {
        const_cast<Stream<P>&>(s).policy_t::operator<<(data);
        return s;
    }

    template <typename T>
    friend const Stream<P>& operator>>(const Stream<P>& s, T& data)
    {
        const_cast<Stream<P>&>(s).policy_t::operator>>(data);
        return s;
    }

    template <typename T>
    friend const Stream<P>& operator, (const Stream<P>& s, const T& data)
    {
        return s << data;
    }

    friend const Stream<P>& operator<<(const Stream<P>& s, Stream<P>& (*opt)(Stream<P>&))
    {
        return opt(const_cast<Stream<P>&>(s));
    }

    friend const Stream<P>& operator, (const Stream<P>& s, Stream<P>& (*opt)(Stream<P>&))
    {
        return opt(const_cast<Stream<P>&>(s));
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
