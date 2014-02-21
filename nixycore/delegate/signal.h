/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/delegate/functor.h"

#include "nixycore/container/deque.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename F>
class signal;

template <typename R>
class signal<R()>
{
public:
    typedef functor<R()> functor_t;

private:
    deque<functor_t> queue_;

public:
    template <typename Fr_>
    void connect(const Fr_& f)
    {
        queue_.push_back(f);
    }

    template <typename Fr_, typename Ob_>
    void connect(Ob_* o, Fr_ f) // object pointer is the first parameter
    {
        queue_.push_back(nx::move(functor_t(f, o)));
    }

    void operator()(void) const
    {
        typename deque<functor_t>::const_iterator ite = queue_.begin();
        for(; ite != queue_.end(); ++ite) (*ite)();
    }

    void clear(void)
    {
        queue_.clear();
    }
};

#define NX_SIGNAL_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class signal<R(NX_PP_TYPE_1(n, P))> \
{ \
public: \
    typedef functor<R(NX_PP_TYPE_1(n, P))> functor_t; \
private: \
    deque<functor_t> queue_; \
public: \
    template <typename Fr_> \
    void connect(const Fr_& f) \
    { \
        queue_.push_back(f); \
    } \
    template <typename Fr_, typename Ob_> \
    void connect(Ob_* o, Fr_ f) \
    { \
        queue_.push_back(nx::move(functor_t(f, o))); \
    } \
    void operator()(NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) const \
    { \
        typename deque<functor_t>::const_iterator ite = queue_.begin(); \
        for(; ite != queue_.end(); ++ite) (*ite)(NX_PP_TYPE_1(n, par)); \
    } \
    void clear(void) \
    { \
        queue_.clear(); \
    } \
};
NX_PP_MULT_MAX(NX_SIGNAL_)
#undef NX_SIGNAL_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
