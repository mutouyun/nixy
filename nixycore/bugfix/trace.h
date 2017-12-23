/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/bugfix/output.h"

#include "nixycore/general/general.h"
#include "nixycore/stream/stream.h"
#include "nixycore/string/string.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class trace : public nx::stream<nx::string>
{
    typedef nx::stream<nx::string> base_t;

    nx::string str_;

public:
    trace(void)
        : base_t(&str_)
    {}

    template <typename S>
    trace(const S& fs)
        : base_t(&str_, fs)
    {}

    ~trace(void)
    {
        base_t::flush();
        nx::output<use::std_cerr>(str_.to_local().c_str());
    }
};

namespace private_trace
{
    class maker
    {
        const nx::string::value_type* str_;

    public:
        maker(const nx::string::value_type* str)
            : str_(str)
        {}

        nx::trace operator()(void)
        {
            return nx::trace(str_);
        }

        template <typename T>
        nx::trace operator()(const T& fmt)
        {
            return nx::trace(nx::string(str_) + fmt);
        }
    };
}

#ifndef NX_ENABLE_TRACE
#ifndef NDEBUG
#   define NX_ENABLE_TRACE
#endif
#endif/*NX_ENABLE_TRACE*/

#ifdef NX_ENABLE_TRACE
#   define nx_trace(...) \
        (nx::private_trace::maker(L"%x\n%x, L: %x ->: ")(__VA_ARGS__) << __FILE__ << NX__FUNCTION__ << __LINE__)
#else /*NX_ENABLE_TRACE*/
#   define nx_trace(...) if (true) ; else nx::trace()
#endif/*NX_ENABLE_TRACE*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
