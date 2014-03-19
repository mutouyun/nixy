/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/pp_macros.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    format string for printf
*/

template <typename T, typename R>
struct printf_format;

#define NX_PRINTF_FORMAT_(R, ...) \
NX_PP_VA(template <> struct printf_format<char   , R> { static const R* val(void) { return __VA_ARGS__##"%c"  ; } }; \
         template <> struct printf_format<uchar  , R> { static const R* val(void) { return __VA_ARGS__##"%c"  ; } }; \
         template <> struct printf_format<wchar  , R> { static const R* val(void) { return __VA_ARGS__##"%lc" ; } }; \
         template <> struct printf_format<short  , R> { static const R* val(void) { return __VA_ARGS__##"%d"  ; } }; \
         template <> struct printf_format<ushort , R> { static const R* val(void) { return __VA_ARGS__##"%u"  ; } }; \
         template <> struct printf_format<int    , R> { static const R* val(void) { return __VA_ARGS__##"%d"  ; } }; \
         template <> struct printf_format<uint   , R> { static const R* val(void) { return __VA_ARGS__##"%u"  ; } }; \
         template <> struct printf_format<long   , R> { static const R* val(void) { return __VA_ARGS__##"%ld" ; } }; \
         template <> struct printf_format<ulong  , R> { static const R* val(void) { return __VA_ARGS__##"%lu" ; } }; \
         template <> struct printf_format<llong  , R> { static const R* val(void) { return __VA_ARGS__##"%lld"; } }; \
         template <> struct printf_format<ullong , R> { static const R* val(void) { return __VA_ARGS__##"%llu"; } }; \
         template <> struct printf_format<float  , R> { static const R* val(void) { return __VA_ARGS__##"%f"  ; } }; \
         template <> struct printf_format<double , R> { static const R* val(void) { return __VA_ARGS__##"%g"  ; } }; \
         template <> struct printf_format<ldouble, R> { static const R* val(void) { return __VA_ARGS__##"%Lg" ; } }; \
         template <> struct printf_format<pchar  , R> { static const R* val(void) { return __VA_ARGS__##"%s"  ; } }; \
         template <> struct printf_format<pwchar , R> { static const R* val(void) { return __VA_ARGS__##"%ls" ; } }; \
         template <> struct printf_format<pvoid  , R> { static const R* val(void) { return __VA_ARGS__##"%p"  ; } };)

NX_PRINTF_FORMAT_(char)
NX_PRINTF_FORMAT_(wchar, L)

#undef NX_PRINTF_FORMAT_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
