/*
    The Nixy Library
    Code covered by the MIT License

    Modified from <<Enhancing Assertions>>
    (http://www.drdobbs.com/cpp/enhancing-assertions/184403745)
    Modified by : mutouyun (http://darkc.at)

    Copyright by Andrei Alexandrescu and John Torjo, August 01, 2003
*/

#pragma once

#include "nixycore/bugfix/output.h"

#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"

#include <map>      // std::map
#include <string>   // std::string
#include <sstream>  // std::ostringstream

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    storage assert context
*/

struct assert_context
{
    NX_STATIC_PROPERTY(int, LEVEL_ABORT , 0);   // will call ::abort() to abort the current process
    NX_STATIC_PROPERTY(int, LEVEL_EXCEPT, 1);   // will throw an exception with an assert_context object
    NX_STATIC_PROPERTY(int, LEVEL_USER  , 2);   // custom define level

    typedef std::string string_type;
    typedef std::map<string_type, string_type> context_vals_type;
    context_vals_type context_vals_;

    const char* expr_;
    int         level_;
    const char* file_;
    unsigned    line_;
    const char* message_;

    assert_context(const char* expr)
        : expr_(expr)
        , level_(LEVEL_ABORT)
        , file_(NULL)
        , line_(0)
        , message_(NULL)
    {}

    template <typename T>
    void set_context_val(const char* str, const T& val)
    {
        try
        {
            std::ostringstream ss;
            ss << val;
            context_vals_[str] = ss.str();
        }
        catch(...) {}
    }

    string_type get_context_val(const char* str) const
    {
        try
        {
            context_vals_type::const_iterator found = context_vals_.find(str);
            if (found != context_vals_.end())
                return found->second;
            else
                return string_type();
        }
        catch(...)
        {
            static string_type s;
            return s;
        }
    }
};

/*
    define assert detail
*/

#ifndef NX_ASSERT_HANDLER
#define NX_ASSERT_HANDLER   &nx::assert_detail::assert_handler
#endif

class assert_detail
{
public:
    typedef void(*handler_t)(assert_context&);

    // default assert handler
    static void assert_handler(assert_context& context)
    {
        nx::output<use::std_cerr>("Failed in \"%s\" ->: %d\n", context.file_, context.line_);
        nx::output<use::std_cerr>("Expression: %s\n", context.expr_);
        nx::output<use::std_cerr>("Context Variables: \n");
        assert_context::context_vals_type::iterator cc = context.context_vals_.begin();
        for(; cc != context.context_vals_.end(); ++cc)
        {
            nx::output<use::std_cerr>("\t%s = %s\n", cc->first.c_str(), cc->second.c_str());
        }
        if (context.message_)
            nx::output<use::std_cerr>("Message: %s\n", context.message_);
        if (context.level_ == assert_context::LEVEL_ABORT)
        {
#       ifdef NX_OS_WINCE
            ::DebugBreak();
#       else
            ::abort();
#       endif
        }
    }

private:
    assert_context context_;
    handler_t handler_;

public:
    assert_detail(const char* expr)
        : context_(expr)
        , handler_(NX_ASSERT_HANDLER)
    {}

    ~assert_detail(void)
    {
        if (context_.level_ == assert_context::LEVEL_ABORT) do_assert();
    }

    assert_detail& do_assert(void)
    {
        handler_(context_);
        return (*this);
    }

    assert_detail& handler(handler_t hd)
    {
        handler_ = hd;
        return (*this);
    }

    assert_detail& context(const char* file, unsigned line)
    {
        context_.file_ = file;
        context_.line_ = line;
        return (*this);
    }

    assert_detail& msg(const char* message)
    {
        context_.message_ = message;
        return (*this);
    }

    template <typename T>
    assert_detail& add(const char* str, const T& val)
    {
        context_.set_context_val(str, val);
        return (*this);
    }

    /*
        Level settings
    */

    assert_detail& level(int lvl)
    {
        switch(lvl)
        {
        case assert_context::LEVEL_ABORT : abort() ; break;
        case assert_context::LEVEL_EXCEPT: except(); break;
        default: context_.level_ = lvl;
        }
        return (*this);
    }

    assert_detail& abort(void)
    {
        context_.level_ = assert_context::LEVEL_ABORT;
        return (*this);
    }

    template <typename T>
    assert_detail& except(const T& exception)
    {
        context_.level_ = assert_context::LEVEL_EXCEPT;
        do_assert();
        throw exception;
        return (*this);
    }

    template <typename T>
    assert_detail& except(void)
    {
        return except(T());
    }

    assert_detail& except(void)
    {
        return except(context_);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
