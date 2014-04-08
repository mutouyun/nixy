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
    typedef stream_wrap<P> warp_t;
    typedef const warp_t& (*option_t)(const warp_t&);

public:
    /*
        operator<<
    */

    template <typename T>
    friend const warp_t& operator<<(const warp_t& s, const T& data)
    {
        const_cast<warp_t&>(s).policy_t::operator<<(data);
        return s;
    }

    template <typename T>
    friend const warp_t& operator>>(const warp_t& s, T& data)
    {
        const_cast<warp_t&>(s).policy_t::operator>>(data);
        return s;
    }

    friend const warp_t& operator<<(const warp_t& s, option_t opt)
    {
        return opt(s);
    }

    /*
        operator,
    */

    template <typename T>
    friend const warp_t& operator,(const warp_t& s, const T& data)
    {
        return s << data;
    }

    friend const warp_t& operator,(const warp_t& s, option_t opt)
    {
        return s << opt;
    }

    /*
        operator()
    */

    template <typename T>
    const warp_t& operator()(const T& data) const
    {
        return (*this) << data;
    }

    template <typename T>
    const warp_t& operator()(option_t opt) const
    {
        return (*this) << opt;
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
