/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/stream/printf_format.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/string/string.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    swprintf & swscanf
*/

template <typename V>
inline int swprintf(nx::string& buf, const wchar* fmt, const V& val)
{
#if defined(NX_OS_WINCE)
#   define NX_SWPRINTF_ ::_snwprintf
#else
#   define NX_SWPRINTF_ ::swprintf
#endif

#if defined(NX_OS_WIN32) || defined(NX_OS_WIN64)
    int n = NX_SWPRINTF_(NULL, 0, fmt, val);
    if (n <= 0) return n;
    size_t eof_index = buf.length();
    buf.resize(eof_index + n);
    n = NX_SWPRINTF_(const_cast<wchar*>(buf.data()) + eof_index,
                     n + 1, fmt, val);
    if (n > 0) buf.resize(eof_index + n);
#else
    nx::wchar tmp[256] = {0};
    int n = NX_SWPRINTF_(tmp, nx_countof(tmp), fmt, val);
    if (n > 0) buf += tmp;
#endif
    return n;

#undef NX_SWPRINTF_
}

template <typename V>
inline int swscanf(const nx::string& buf, const wchar* fmt, V& val)
{
    return ::swscanf(buf.c_str(), fmt, &val);
}

/*
    buffer for streaming in or out
*/

template <class T>
class stream_buffer
{
    T*     ref_;
    string fmt_, buf_;
    size_t index_, ref_index_;

    enum mode_t
    {
        mode_Non,
        mode_In,
        mode_Out
    } mode_;

public:
    bool is_read_complete(void) const
    {
        nx_assert(ref_);
        if (mode_ == mode_In)
            return (ref_index_ >= ref_->length());
        else
            return false;
    }

    bool is_finished(void) const
    {
        return (index_ >= fmt_.length()) || is_read_complete();
    }

private:
#   include "detail/stream_buffer_out.hxx"
#   include "detail/stream_buffer_in.hxx"

public:
    stream_buffer(void)
        : ref_      (nx::nulptr)
        , index_    (0)
        , ref_index_(0)
        , mode_     (mode_Non)
    {}

    stream_buffer(const stream_buffer& r)
        : ref_      (r.ref_)
        , fmt_      (r.fmt_)
        , buf_      (r.buf_)
        , index_    (r.index_)
        , ref_index_(r.ref_index_)
        , mode_     (r.mode_)
    {}

    stream_buffer(nx_rref(stream_buffer, true) r)
        : ref_      (nx::nulptr)
        , index_    (0)
        , ref_index_(0)
        , mode_     (mode_Non)
    {
        swap(nx::moved(r));
    }

    ~stream_buffer(void)
    {
        flush();
    }

    void swap(stream_buffer& r)
    {
        nx::swap(ref_       , r.ref_);
        nx::swap(fmt_       , r.fmt_);
        nx::swap(buf_       , r.buf_);
        nx::swap(index_     , r.index_);
        nx::swap(ref_index_ , r.ref_index_);
        nx::swap(mode_      , r.mode_);
    }

public:
    void set_refer(T* tp)
    {
        nx_assert(tp);
        ref_ = tp;
    }

    template <typename S>
    void set_format(T* tp, const S& fs)
    {
        set_refer(tp);
        fmt_ = fs;
        buf_.clear();
    }

    void flush(void)
    {
        if (!ref_) return;
        if (mode_ == mode_Out)
        {
            if (!is_finished()) return;
            (*ref_) = nx::move(buf_);
        }
        else
        if (mode_ == mode_In)
        {
            // do nothing
        }
        else
        {
            do_assign_out();
            if (!is_finished()) return;
            (*ref_) = nx::move(buf_);
        }
        ref_ = nx::nulptr;
    }

    template <typename V>
    void operator<<(const V& val)
    {
        if (mode_ == mode_In) return;
        if (mode_ == mode_Non)
        {
            mode_ = mode_Out;
            do_assign_out();
        }
        parse_out(val);
        do_assign_out();
    }

    template <typename V>
    void operator>>(V& val)
    {
        if (mode_ == mode_Out) return;
        if (mode_ == mode_Non)
        {
            mode_ = mode_In;
            do_assign_in();
        }
        parse_in(val);
        do_assign_in();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
