/*
    The Nixy Library
    Code covered by the MIT License

    Modified from <<Enhancing Assertions>>
    (http://www.drdobbs.com/cpp/enhancing-assertions/184403745)
    Modified by : mutouyun (http://darkc.at)

    Copyright by Andrei Alexandrescu and John Torjo, August 01, 2003
*/

#pragma once

#include "nixycore/bugfix/noexcept.h"
#include "nixycore/bugfix/output.h"

#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"

#include <map>          // std::map
#include <string>       // std::string
#include <sstream>      // std::ostringstream
#include <exception>    // std::exception

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    storage assert context
*/

struct assert_context : std::exception
{
    NX_STATIC_PROPERTY(int, LEVEL_ABORT , 0);   // will call ::abort() to abort the current process
    NX_STATIC_PROPERTY(int, LEVEL_EXCEPT, 1);   // will throw an exception with an assert_context object
    NX_STATIC_PROPERTY(int, LEVEL_USER  , 2);   // custom define level

    typedef std::string        string_type;
    typedef std::ostringstream stream_type;
    typedef std::map<string_type, string_type> context_vals_type;
    context_vals_type context_vals_;

    const char* expr_;
    int         level_;
    const char* file_;
    unsigned    line_;
    const char* message_;

    string_type what_;

    assert_context(const char* expr) nx_noexcept
        : expr_(expr)
        , level_(LEVEL_ABORT)
        , file_(NULL)
        , line_(0)
        , message_(NULL)
    {}

    virtual ~assert_context(void) nx_noexcept
    {}

    template <typename T>
    void set_context_val(const char* str, const T& val) nx_noexcept
    {
        try
        {
            stream_type ss;
            ss << val;
            context_vals_[str] = ss.str();
        }
        catch(...) {}
    }

    const char* get_context_val(const char* str) const nx_noexcept
    {
        try
        {
            context_vals_type::const_iterator found = context_vals_.find(str);
            if (found != context_vals_.end())
                return found->second.c_str();
            else
                return "";
        }
        catch(...)
        {
            return "";
        }
    }

    void flush(void) nx_noexcept
    {
        try
        {
            stream_type ss;
            ss << "Failed in \"" << file_ << "\" ->: " << line_ << std::endl;
            ss << "Expression: \"" << expr_ << "\"" << std::endl;
            ss << "Context Variables: " << std::endl;
            context_vals_type::iterator cc = context_vals_.begin();
            for(; cc != context_vals_.end(); ++cc)
            {
                ss << "\t" << cc->first << " = " << cc->second << std::endl;
            }
            if (message_)
            {
                ss << "Message: " << message_ << std::endl;
            }
            what_ = ss.str();
        }
        catch(...) {}
    }

    virtual const char* what() const nx_noexcept
    {
        try
        {
            return what_.c_str();
        }
        catch(...)
        {
            return "";
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
        nx::output<use::std_cerr>(context.what());
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
    assert_detail(const char* expr) nx_noexcept
        : context_(expr)
        , handler_(NX_ASSERT_HANDLER)
    {}

    ~assert_detail(void) nx_noexcept
    {
        if (context_.level_ == assert_context::LEVEL_ABORT) do_assert();
    }

    assert_detail& do_assert(void) nx_noexcept
    {
        context_.flush();
        handler_(context_);
        return (*this);
    }

    assert_detail& handler(handler_t hd) nx_noexcept
    {
        handler_ = hd;
        return (*this);
    }

    assert_detail& context(const char* file, unsigned line) nx_noexcept
    {
        context_.file_ = file;
        context_.line_ = line;
        return (*this);
    }

    assert_detail& msg(const char* message) nx_noexcept
    {
        context_.message_ = message;
        return (*this);
    }

    template <typename T>
    assert_detail& add(const char* str, const T& val) nx_noexcept
    {
        context_.set_context_val(str, val);
        return (*this);
    }

    /*
        Level settings
    */

    assert_detail& level(int lvl) nx_noexcept
    {
        switch(lvl)
        {
        case assert_context::LEVEL_ABORT : abort() ; break;
        case assert_context::LEVEL_EXCEPT: except(); break;
        default: context_.level_ = lvl;
        }
        return (*this);
    }

    assert_detail& abort(void) nx_noexcept
    {
        context_.level_ = assert_context::LEVEL_ABORT;
        return (*this);
    }

    template <typename T>
    assert_detail& except(const T& ept)
    {
        context_.level_ = assert_context::LEVEL_EXCEPT;
        do_assert();
        throw ept;
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
