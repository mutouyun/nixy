/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/stream/stream_buffer.h"
#include "nixycore/stream/stream_wrap.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <class T>
class stream : public stream_wrap<stream_buffer<T> >
{
    typedef stream_wrap<stream_buffer<T> > base_t;

public:
    template <typename S>
    stream(T* tp, const S& fs)
    {
        base_t::set_format(tp, fs);
    }

    stream(T* tp)
    {
        base_t::set_format(tp, L"");
    }
};

/*
    Make a stream for in or out
*/

template <class T, typename S>
stream<T> format(T* tp, const S& fs)
{
    return stream<T>(tp, fs);
}

template <class T>
stream<T> io(T* tp)
{
    return stream<T>(tp);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
