/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
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

template <class T>
class stream_buffer : noncopyable
{
    T*     ref_;
    string fmt_, buf_;
    size_t index_, ref_index_;

    enum mode_t
    {
        mode_Non,
        mode_In,
        mode_Out
    };
    mode_t mode_;

public:
    void swap(stream_buffer& r)
    {
        nx::swap(ref_, r.ref_);
        fmt_.swap(r.fmt_);
        buf_.swap(r.buf_);
        nx::swap(index_, r.index_);
    }

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
#   include "nixycore/stream/stream_buffer_out.hxx"
#   include "nixycore/stream/stream_buffer_in.hxx"

public:
    stream_buffer(void)
        : ref_(nx::nulptr)
        , index_(0), ref_index_(0)
        , mode_(mode_Non)
    {}

    ~stream_buffer()
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
    }

public:
    template <typename S>
    void set_format(T* tp, const S& fs)
    {
        nx_assert(tp);
        ref_ = tp;
        fmt_ = fs;
        buf_.clear();
    }

    template <typename V>
    void operator<<(const V& val)
    {
        if (mode_ == mode_In) return;
        if (mode_ == mode_Non) mode_ = mode_Out;
        do_assign_out();
        parse_out(val);
    }

    template <typename V>
    void operator>>(V& val)
    {
        if (mode_ == mode_Out) return;
        if (mode_ == mode_Non) mode_ = mode_In;
        do_assign_in();
        parse_in(val);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
