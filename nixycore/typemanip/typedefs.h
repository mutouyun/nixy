/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/preprocessor/pp_macros.h"

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_NULLPTR
#include <cstddef> // std::nullptr_t
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    null_t
*/

struct null_t;

/*
    nulptr_t && nulptr
*/

#ifdef NX_SP_CXX11_NULLPTR
typedef std::nullptr_t nulptr_t;
#else /*NX_SP_CXX11_NULLPTR*/
class nulptr_t
{
public:
    template <typename T>
    inline operator T*() const
    { return 0; }

    template <class C, class T>
    inline operator T C::*() const
    { return 0; }

private:
    void operator&() const;
};
#endif/*NX_SP_CXX11_NULLPTR*/

static const nulptr_t nulptr = nulptr_t();

/*
    empty_t && none_t && none
*/

struct empty_t {};
typedef int empty_t::*none_t;
static const none_t none = nx::nulptr;

/*
    true_t && false_t
*/

#define NX_STATIC_PROPERTY(T, name, ...) \
    static const T name = (__VA_ARGS__)

#define NX_STATIC_VALUE(T, ...) NX_STATIC_PROPERTY(T, value, __VA_ARGS__)

template <typename T, T Val_>
struct ValueDef { NX_STATIC_VALUE(T, Val_); };

typedef ValueDef<bool, true>    true_t;
typedef ValueDef<bool, false>   false_t;

/*
    yes_t && not_t
*/

typedef char not_t;
typedef struct { not_t dummy_[2]; } yes_t;

#ifndef nx_judge
#define nx_judge(...) NX_PP_VA(nx::ValueDef<bool, sizeof(__VA_ARGS__) == sizeof(nx::yes_t)>::value)
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
