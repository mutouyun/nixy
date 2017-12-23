/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <class PolicyT>
class stream_wrap : public PolicyT
{
public:
    typedef PolicyT policy_t;
    typedef stream_wrap<policy_t> warp_t;
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
