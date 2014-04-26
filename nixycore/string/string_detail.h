/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/string/transform.h"

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"
#include "nixycore/container/vector.h"

#include "nixycore/delegate/functor.h"
#include "nixycore/delegate/bind.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

// std::basic_string
#include <string>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_string
{
    typedef std::basic_string<char, std::char_traits<char>,
            NX_DEFAULT_ALLOC::std_allocator<char>::type_t>  a_base_t;
    typedef std::basic_string<wchar, std::char_traits<wchar>,
            NX_DEFAULT_ALLOC::std_allocator<wchar>::type_t> w_base_t;
}

/*
    The local character set string
*/

class local_string : public private_string::a_base_t
{
    typedef private_string::a_base_t base_t;

public:
    local_string(void)
        : base_t()
    {}

    local_string(const char* str)
        : base_t(str)
    {}

    local_string(char c)
        : base_t()
    {
        base_t::push_back(c);
    }

    local_string(const local_string& rhs)
        : base_t(rhs)
    {}

    local_string(nx_rref(local_string) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }

    local_string& operator =(local_string rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    The unicode string
*/

class string : protected private_string::w_base_t
{
    typedef private_string::w_base_t base_t;

public:
    typedef base_t::size_type              size_type;
    typedef base_t::value_type             value_type;
    typedef base_t::iterator               iterator;
    typedef base_t::reverse_iterator       reverse_iterator;
    typedef base_t::const_iterator         const_iterator;
    typedef base_t::const_reverse_iterator const_reverse_iterator;

public:
    string(void)
        : base_t()
    {}

    string(const char* str)
        : base_t()
    {
        from_local(str);
    }

    string(const char* str, size_type n)
        : base_t()
    {
        from_local(str, n);
    }

    string(const value_type* wcs)
        : base_t(wcs)
    {}

    string(const value_type* wcs, size_type n)
        : base_t(wcs, n)
    {}

    string(value_type wc, size_type n = 1)
        : base_t(n, wc)
    {}

    template <typename IteratorT>
    string(IteratorT f, IteratorT l)
        : base_t(f, l)
    {}

    string(const string& rhs, size_type pos = 0, size_type n = npos)
        : base_t(rhs, pos, n)
    {}

    string(nx_rref(string) rhs)
        : base_t()
    {
        swap(nx::moved(rhs));
    }

public:
    /*
        assignment
    */

    string& operator =(string rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

    /*
        append
    */

    string& operator +=(const char* rhs)
    {
        base_t::operator +=(string(rhs));
        return (*this);
    }

    template <typename T>
    string& operator +=(const T& rhs)
    {
        base_t::operator +=(rhs);
        return (*this);
    }

    /*
        link
    */

    friend nx_rval(string) operator+(const string& x, const string& y)
    {
        return nx::move(string(x) += y);
    }

    friend nx_rval(string) operator+(nx_rref(string) x, const string& y)
    {
        return nx::move(moved(x) += y);
    }

    friend nx_rval(string) operator+(const string& x, const value_type* y)
    {
        return nx::move(string(x) += y);
    }

    friend nx_rval(string) operator+(const value_type* x, const string& y)
    {
        return nx::move(string(x) += y);
    }

    friend nx_rval(string) operator+(nx_rref(string) x, const value_type* y)
    {
        return nx::move(moved(x) += y);
    }

    /*
        compare
    */

    friend bool operator==(const string& x, const string& y)
    {
        return (x.compare(y) == 0);
    }

    friend bool operator==(const string& x, const value_type* y)
    {
        return (x.compare(y) == 0);
    }

    friend bool operator==(const value_type* x, const string& y)
    {
        return (y.compare(x) == 0);
    }

    friend bool operator!=(const string& x, const string& y)
    {
        return (x.compare(y) != 0);
    }

    friend bool operator!=(const string& x, const value_type* y)
    {
        return (x.compare(y) != 0);
    }

    friend bool operator!=(const value_type* x, const string& y)
    {
        return (y.compare(x) != 0);
    }

    /*
        character set transform
    */

    string& from_local(const char* str)
    {
        size_t n = transform::local_to_utf(str);
        if (n == 0) return (*this);
        resize(n);
        transform::local_to_utf(str, const_cast<value_type*>(data()), n);
        resize(n - 1);
        return (*this);
    }

    string& from_local(const char* str, size_type len)
    {
        if (!str || len == 0) return (*this);
        char& c = const_cast<char*>(str)[len];
        char t = c; // backup
        c = 0;
        from_local(str);
        c = t;      // restore
        return (*this);
    }

    string& from_local(const local_string& str)
    {
        return from_local(str.c_str());
    }

    nx_rval(local_string) to_local(void) const
    {
        local_string str;
        size_t n = transform::utf_to_local(c_str());
        if (n == 0) return str;
        str.resize(n);
        transform::utf_to_local(c_str(), const_cast<char*>(str.data()), n);
        str.resize(n - 1);
        return nx::move(str);
    }

    /*
        for stream
    */

    friend void operator<<(string& buf, const string& val) // out
    {
        buf.append(val);
    }

    friend void operator>>(const string& buf, string& val) // in
    {
        val = buf;
    }

    /*
        others
    */

    size_type size(void) const
    {
        return base_t::capacity();
    }

    void swap(string& y)
    {
        base_t::swap(y);
    }

    /*
        string operations
    */

#   include "detail/string_ops_define.hxx"

public:
    using base_t::npos;
    using base_t::resize;
    using base_t::length;
    using base_t::empty;
    using base_t::clear;
    using base_t::assign;
    using base_t::append;
    using base_t::insert;
    using base_t::erase;
    using base_t::compare;
    using base_t::find;
    using base_t::rfind;
    using base_t::reserve;
    using base_t::replace;
    using base_t::begin;
    using base_t::end;
    using base_t::rbegin;
    using base_t::rend;
    using base_t::push_back;
    using base_t::data;
    using base_t::c_str;
    using base_t::at;
    using base_t::operator [];
};

/*
    Special swap algorithm
*/

inline void swap(string& x, string& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
