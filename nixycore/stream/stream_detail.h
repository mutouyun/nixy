/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
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

    stream(const rvalue<stream, true>& r)
    {
        base_t::swap(nx::unmove(r));
    }
};

/*
    Make a stream for in or out
*/

template <class T, typename S>
stream<T> format(T* tp, const S& fs)
{
    return nx::move(stream<T>(tp, fs));
}

template <class T>
stream<T> make_stream(T* tp)
{
    return nx::move(stream<T>(tp));
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
