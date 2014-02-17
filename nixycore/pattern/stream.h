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
class stream : public P
{
public:
    typedef P policy_t;

public:
    template <typename T>
    friend const stream<P>& operator<<(const stream<P>& s, const T& data)
    {
        const_cast<stream<P>&>(s).policy_t::operator<<(data);
        return s;
    }

    template <typename T>
    friend const stream<P>& operator>>(const stream<P>& s, T& data)
    {
        const_cast<stream<P>&>(s).policy_t::operator>>(data);
        return s;
    }

    template <typename T>
    friend const stream<P>& operator, (const stream<P>& s, const T& data)
    {
        return s << data;
    }

    friend const stream<P>& operator<<(const stream<P>& s, stream<P>& (*opt)(stream<P>&))
    {
        return opt(const_cast<stream<P>&>(s));
    }

    friend const stream<P>& operator, (const stream<P>& s, stream<P>& (*opt)(stream<P>&))
    {
        return opt(const_cast<stream<P>&>(s));
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
