/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/delegate/functor.h"

#include "nixycore/container/deque.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename F>
class signal;

#ifdef NX_SP_CXX11_TEMPLATES
template <typename R, typename... P>
class signal<R(P...)>
{
public:
    typedef functor<R(P...)> functor_t;

private:
    nx::deque<functor_t> queue_;

public:
    template <typename FuncT>
    void connect(nx_fref(FuncT) f)
    {
        queue_.push_back(nx_forward(FuncT, f));
    }

    template <typename FuncT, typename ObjT>
    void connect(ObjT* o, nx_fref(FuncT) f)
    {
        queue_.push_back(nx::move(functor_t(nx_forward(FuncT, f), o)));
    }

    template <typename... P_>
    void operator()(nx_fref(P_)... par) const
    {
        typename deque<functor_t>::const_iterator ite = queue_.begin();
        for(; ite != queue_.end(); ++ite) (*ite)(nx_forward(P_, par)...);
    }

    void clear(void)
    {
        queue_.clear();
    }
};
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename R>
class signal<R()>
{
public:
    typedef functor<R()> functor_t;

private:
    deque<functor_t> queue_;

public:
    template <typename FuncT>
    void connect(nx_fref(FuncT) f)
    {
        queue_.push_back(nx_forward(FuncT, f));
    }

    template <typename FuncT, typename ObjT>
    void connect(ObjT* o, nx_fref(FuncT) f) // object pointer is the first parameter
    {
        queue_.push_back(nx::move(functor_t(nx_forward(FuncT, f), o)));
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

#define NX_SIGNAL_DEFINE_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class signal<R(NX_PP_TYPE_1(n, P))> \
{ \
public: \
    typedef functor<R(NX_PP_TYPE_1(n, P))> functor_t; \
private: \
    deque<functor_t> queue_; \
public: \
    template <typename FuncT> \
    void connect(nx_fref(FuncT) f) \
    { \
        queue_.push_back(nx_forward(FuncT, f)); \
    } \
    template <typename FuncT, typename ObjT> \
    void connect(ObjT* o, nx_fref(FuncT) f) \
    { \
        queue_.push_back(nx::move(functor_t(nx_forward(FuncT, f), o))); \
    } \
    template <NX_PP_TYPE_1(n, typename P_)> \
    void operator()(NX_PP_TYPE_2(n, P_, NX_PP_FREF(par))) const \
    { \
        typename deque<functor_t>::const_iterator ite = queue_.begin(); \
        for(; ite != queue_.end(); ++ite) (*ite)(NX_PP_FORWARD(n, P_, par)); \
    } \
    void clear(void) \
    { \
        queue_.clear(); \
    } \
};
NX_PP_MULT_MAX(NX_SIGNAL_DEFINE_)
#undef NX_SIGNAL_DEFINE_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
