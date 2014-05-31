/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/string/transform.h"
#include "nixycore/utility/countof.h"

#include "nixycore/general/general.h"

// std::cout, std::cerr, std::clog
#include <iostream>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace use
{
    struct std_cout
    {
        static void out(const char* str)
        {
            std::cout << str;
        }
    };

    struct std_cerr
    {
        static void out(const char* str)
        {
#       if defined(NX_OS_WINCE)
            nx::wchar wcs[MAX_PATH];
            transform::local_to_utf(str, wcs, nx_countof(wcs));
            ::OutputDebugStringW(wcs);
#       else
#           if defined(NX_OS_WIN)
            ::OutputDebugStringA(str);
#           endif
            std::cerr << str;
#       endif
        }
    };

    struct std_clog
    {
        static void out(const char* str)
        {
            std::clog << str;
        }
    };
}

template <class PolicyT>
bool output(const char* fmt, ...)
{
    if (!fmt) return false;
    bool ret = true;
    va_list args;
    va_start(args, fmt);
#ifdef NX_OS_WINCE
    char buf[MAX_PATH];
    if (::_vsnprintf(buf, nx_countof(buf), fmt, args) > 0)
    {
        PolicyT::out(buf);
    }
    else ret = false;
#else
    char* buf = static_cast<char*>(::malloc(::vsnprintf(NULL, 0, fmt, args) + 1));
    if (buf)
    {
        if (::vsprintf(buf, fmt, args) > 0)
        {
            PolicyT::out(buf);
        }
        else ret = false;
        ::free(buf);
    }
    else ret = false;
#endif
    va_end(args);
    return ret;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
