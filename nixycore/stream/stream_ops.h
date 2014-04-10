/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/stream/stream_detail.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/string/string.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Print "\n" to the stream, just like std::endl
*/

template <class P>
const stream_wrap<P>& endl(const stream_wrap<P>& ss)
{
    return ss << L"\n";
}

template <size_t N, class P>
const stream_wrap<P>& endl(const stream_wrap<P>& ss)
{
    for(size_t i = 0; i < N; ++i)
        ss << L"\n";
    return ss;
}

/*
    Custom format
*/

template <typename T>
class custom_format
{
    nx::string fmt_;
    T ref_;

public:
    template <typename S>
    custom_format(const S& fmt, T r)
        : fmt_(fmt)
        , ref_(r)
    {}

    custom_format(const rvalue<custom_format, true> rhs)
        : ref_(nx::unmove(rhs).ref_)
    {
        nx::unmove(rhs).fmt_.swap(fmt_);
    }

    friend void operator<<(string& buf, const custom_format<T>& val) // out
    {
        nx::swprintf(buf, val.fmt_.c_str(), val.ref_);
    }

    friend void operator>>(const string& buf, custom_format<T>& val) // in
    {
        nx::swscanf(buf, val.fmt_.c_str(), val.ref_);
    }
};

template <typename T, typename S>
custom_format<const T&> fmt(const S& fmt, const T& val)
{
    return custom_format<const T&>(fmt, val);
}

template <typename T, typename S>
custom_format<T&> fmt(const S& fmt, T& val)
{
    return custom_format<T&>(fmt, val);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
