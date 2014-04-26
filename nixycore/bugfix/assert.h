/*
    The Nixy Library
    Code covered by the MIT License

    Modified from <<Enhancing Assertions>>
    (http://www.drdobbs.com/cpp/enhancing-assertions/184403745)
    Modified by : mutouyun (http://darkc.at)

    Copyright by Andrei Alexandrescu and John Torjo, August 01, 2003
*/

#pragma once

#include "nixycore/bugfix/assert_detail.h"

#include "nixycore/preprocessor/pp_macros.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    assert && verify && ensure
*/

namespace private_assert
{
#if defined(NX_CC_MSVC)
#   pragma warning(push)            // <MSVC 2005>
#   pragma warning(disable: 4355)   // 'this' : used in base member initializer list
#endif

    template <bool>
    class detail : public assert_detail
    {
    public:
        using assert_detail::handler_t;

        detail& NX_ASSERT_1_;
        detail& NX_ASSERT_2_;

        detail(const char* expr)
            : assert_detail(expr)
            , NX_ASSERT_1_(*this)
            , NX_ASSERT_2_(*this)
        {}

        detail& context(const char* file, unsigned line)
        {
            return static_cast<detail&>(assert_detail::context(file, line));
        }

        template <typename T>
        detail& add(const char* str, const T& val)
        {
            return static_cast<detail&>(assert_detail::add(str, val));
        }
    };

    template <>
    class detail<false> : public assert_detail
    {
    public:
        using assert_detail::handler_t;

        detail& NX_ASSERT_1_;
        detail& NX_ASSERT_2_;

        /*
            Let the compiler can automatically
            make detail<false> invalid
        */

        detail(const char* /*expr*/)
            : assert_detail("")
            , NX_ASSERT_1_(*this)
            , NX_ASSERT_2_(*this)
        {}

        detail& context(const char* /*file*/, unsigned /*line*/)
        {
            return (*this);
        }

        template <typename T>
        detail& add(const char* /*str*/, const T& /*val*/)
        {
            return (*this);
        }
    };

#if defined(NX_CC_MSVC)
#   pragma warning(pop)
#endif
}

#define NX_ASSERT_1_(x)  NX_ASSERT_OP_(x, 2_)
#define NX_ASSERT_2_(x)  NX_ASSERT_OP_(x, 1_)
#define NX_ASSERT_OP_(x, next) \
        NX_ASSERT_1_.add(#x, (x)).NX_ASSERT_##next

#define nx_ensure(...) \
        if (!!(__VA_ARGS__)) ; \
        else const nx::assert_detail& NX_UNUSED dummy_ = \
        nx::private_assert::detail<true>(#__VA_ARGS__).context(__FILE__, __LINE__).NX_ASSERT_1_

#ifdef NDEBUG
#   define nx_assert(...) \
        if (true) ; \
        else const nx::assert_detail& NX_UNUSED dummy_ = \
        nx::private_assert::detail<false>(#__VA_ARGS__).context(__FILE__, __LINE__).NX_ASSERT_1_
#else /*NDEBUG*/
#   define nx_assert(...) nx_ensure(__VA_ARGS__)
#endif/*NDEBUG*/

#ifdef NDEBUG
#   define nx_verify(...) \
        if (!!(__VA_ARGS__) || true) ; \
        else const nx::assert_detail& NX_UNUSED dummy_ = \
        nx::private_assert::detail<false>(#__VA_ARGS__).context(__FILE__, __LINE__).NX_ASSERT_1_
#else
#   define nx_verify(...) nx_assert(__VA_ARGS__)
#endif/*NDEBUG*/

//////////////////////////////////////////////////////////////////////////

/*
    static assert
*/

#ifdef NX_SP_CXX11_SATIC_ASSERT
#define nx_assert_static(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#else
template <bool, int> struct static_assert_failure;
template <int LineN> struct static_assert_failure<true, LineN> {};
#define nx_assert_static(...) \
    enum \
    { \
        NX_PP_JOIN(static_assert_failure_test_, __LINE__) = \
            sizeof(nx::static_assert_failure<!!(__VA_ARGS__), __LINE__>) \
    }
#endif/*NX_SP_CXX11_SATIC_ASSERT*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
