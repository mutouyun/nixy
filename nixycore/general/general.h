/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

//////////////////////////////////////////////////////////////////////////

/*
    Standard Librarys
*/

// size_t, ptrdiff_t, ...
#include <stddef.h>
// printf, vsnprintf, vsprintf, ...
#include <stdio.h>
// wprintf, vswprintf, ...
#include <wchar.h>
// abort, rand, srand, ...
#include <stdlib.h>
// va_list, va_start, va_end
#include <stdarg.h>
// memcpy, memset, ...
#include <string.h>

/*
    Detect platforms
*/

// NX_OS, NX_CC, NDEBUG, ...
#include "nixycore/general/plat_detect.h"

//////////////////////////////////////////////////////////////////////////

// namespace define

#define NX_BEG              namespace nx {
#define NX_END              }

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

// global types

typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef long long           llong;
typedef unsigned long long  ullong;

typedef wchar_t             wchar;
typedef long double         ldouble;

typedef char*               pchar;
typedef const char*         cpchar;
typedef wchar_t*            pwchar;
typedef const wchar_t*      cpwchar;
typedef void*               pvoid;
typedef const void*         cpvoid;

typedef unsigned char       byte;

typedef ::size_t            size_t;
typedef ::ptrdiff_t         ptrdiff_t;

typedef char                sint8;
typedef uchar               uint8;
typedef short               sint16;
typedef ushort              uint16;
typedef int                 sint32;
typedef uint                uint32;
typedef llong               sint64;
typedef ullong              uint64;

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#include "nixycore/general/global_functions.h"

//////////////////////////////////////////////////////////////////////////
