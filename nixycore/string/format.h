/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    format string for printf
*/

template <typename T, typename R>
struct format;

#define NX_PRINTF_FORMAT_(R, ...) \
template <> struct format<char   , R> { static const R* val(void) { return __VA_ARGS__##"%c"  ; } }; \
template <> struct format<uchar  , R> { static const R* val(void) { return __VA_ARGS__##"%c"  ; } }; \
template <> struct format<wchar  , R> { static const R* val(void) { return __VA_ARGS__##"%lc" ; } }; \
template <> struct format<short  , R> { static const R* val(void) { return __VA_ARGS__##"%d"  ; } }; \
template <> struct format<ushort , R> { static const R* val(void) { return __VA_ARGS__##"%u"  ; } }; \
template <> struct format<int    , R> { static const R* val(void) { return __VA_ARGS__##"%d"  ; } }; \
template <> struct format<uint   , R> { static const R* val(void) { return __VA_ARGS__##"%u"  ; } }; \
template <> struct format<long   , R> { static const R* val(void) { return __VA_ARGS__##"%ld" ; } }; \
template <> struct format<ulong  , R> { static const R* val(void) { return __VA_ARGS__##"%lu" ; } }; \
template <> struct format<llong  , R> { static const R* val(void) { return __VA_ARGS__##"%lld"; } }; \
template <> struct format<ullong , R> { static const R* val(void) { return __VA_ARGS__##"%llu"; } }; \
template <> struct format<float  , R> { static const R* val(void) { return __VA_ARGS__##"%f"  ; } }; \
template <> struct format<double , R> { static const R* val(void) { return __VA_ARGS__##"%g"  ; } }; \
template <> struct format<ldouble, R> { static const R* val(void) { return __VA_ARGS__##"%Lg" ; } }; \
template <> struct format<pchar  , R> { static const R* val(void) { return __VA_ARGS__##"%s"  ; } }; \
template <> struct format<pwchar , R> { static const R* val(void) { return __VA_ARGS__##"%ls" ; } }; \
template <> struct format<pvoid  , R> { static const R* val(void) { return __VA_ARGS__##"%p"  ; } };

NX_PRINTF_FORMAT_(char)
NX_PRINTF_FORMAT_(wchar, L)

#undef NX_FORMAT_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
