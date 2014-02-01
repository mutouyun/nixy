/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/alloc.h"
#include "pattern/prototype.h"

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "utility/utility.h"
#include "algorithm/algorithm.h"
#include "preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_functor
{
    /*
        The handler for storing a function
    */

    struct handler
    {
        pvoid p_;

        handler(void)
            : p_(nx::nulptr)
        {}

        template <typename Tp_>
        handler(Tp_ ptr)
            : p_(nx::nulptr)
        {
            // ptr may not be a pointer
            std::memcpy(&p_, &ptr, nx_min(sizeof(pvoid), sizeof(Tp_)));
        }

        handler(const handler& hr)
            : p_(hr.p_)
        {}

        void swap(handler& r)
        {
            nx::swap(p_, r.p_);
        }

        friend bool operator==(const handler& x, const handler& y)
        {
            return nx::equal(x.p_, y.p_);
        }

        handler& operator=(const handler& hr)
        {
            p_ = hr.p_;
            return (*this);
        }

        template <typename Tp_>
        handler& operator=(Tp_ ptr)
        {
            // ptr may not be a pointer
            std::memcpy(&p_, &ptr, nx_min(sizeof(pvoid), sizeof(Tp_)));
            return (*this);
        }

        template <typename Tp_>
        typename enable_if<is_pointer<Tp_>::value, 
        Tp_>::type_t cast(void)
        {
            return (Tp_)(p_);
        }

        template <typename Tp_>
        typename enable_if<!is_pointer<Tp_>::value, 
        Tp_*>::type_t cast(void)
        {
            return (Tp_*)&(p_);
        }
    };

    /*
        The invoker for calling a function
    */

    template <typename F, typename Func_>
    struct invoker;

    template <typename Func_>
    struct invoker<void(), Func_>
    {
        static void invoke(handler& hr)
        {
            ( *(hr.cast<Func_>()) )(); // the Func_'s return value may not be void
        }
    };

    template <typename R, typename Func_>
    struct invoker<R(), Func_>
    {
        static R invoke(handler& hr)
        {
            return ( *(hr.cast<Func_>()) )();
        }
    };

#define NX_FUNCTOR_INVOKER_(n) \
    template <NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), Func_> \
    { \
        static void invoke(handler& hr, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            ( *(hr.cast<Func_>()) )(NX_PP_TYPE_1(n, par)); \
        } \
    };
    NX_PP_MULT_MAX(NX_FUNCTOR_INVOKER_)
#undef NX_FUNCTOR_INVOKER_

#define NX_FUNCTOR_INVOKER_(n) \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), Func_> \
    { \
        static R invoke(handler& hr, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return ( *(hr.cast<Func_>()) )(NX_PP_TYPE_1(n, par)); \
        } \
    };
    NX_PP_MULT_MAX(NX_FUNCTOR_INVOKER_)
#undef NX_FUNCTOR_INVOKER_
}

/*
    The base class of functor
*/

template <typename Style_, typename Functor_, typename Invoker_>
class functor_base : public SafeBool<functor_base<Style_, Functor_, Invoker_> >
                   , public Unequal <Functor_>
{
public:
    typedef Style_   style_type;
    typedef Functor_ functor_type;
    typedef Invoker_ invoker_type;

protected:
    private_functor::handler handler_;
    invoker_type             invoker_;

private:
    class PlaceHolder
    {
    public:
        virtual ~PlaceHolder() {}

    public:
        virtual pvoid handler(void) const = 0;
        virtual PlaceHolder* clone(void) = 0;
    } * any_guard_;

    template <typename T>
    class Holder : public PlaceHolder
    {
    public:
        T held_;

    public:
        Holder(const T& v): held_(v) {}

    public:
        pvoid handler(void) const { return (pvoid)nx::addressof(held_); }
        PlaceHolder* clone(void) { return nx::alloc<Holder>(nx::ref(held_)); }
    };

protected:
    template <typename T>
    T* guard(const T& f)
    {
        any_guard_ = nx::alloc<Holder<T> >(nx::ref(f));
        return (T*)(any_guard_->handler());
    }

public:
    functor_base(void)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {}

    template <typename Func_>
    functor_base(const Func_& f)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    { set(f); }

    functor_base(const functor_base& fr)
        : invoker_(fr.invoker_), any_guard_(nx::nulptr)
    {
        if (fr.any_guard_)
        {
            any_guard_ = nx::clone(fr.any_guard_);
            handler_ = any_guard_->handler();
        }
        else
            handler_ = fr.handler_;
    }

    ~functor_base()
    { nx::free(any_guard_); }

public:
    bool checkSafeBool(void) const
    {
        return !!invoker_;
    }

    void swap(functor_type& r)
    {
        nx::swap(handler_, r.handler_);
        nx::swap(invoker_, r.invoker_);
        nx::swap(any_guard_, r.any_guard_);
    }

    functor_type& operator=(functor_type fr)
    {
        fr.swap(*reinterpret_cast<functor_type*>(this));
        return (*reinterpret_cast<functor_type*>(this));
    }

    friend bool operator==(const functor_type& f1, const functor_type& f2)
    {
        return nx::equal(f1.handler_, f2.handler_);
    }

protected:
    template <typename Func_>
    void assign_to(Func_ f)
    {
        handler_ = f;
        invoker_ = &private_functor::invoker<style_type, Func_>::invoke;
    }

    template <typename Func_>
    typename nx::enable_if<nx::is_pointer<Func_>::value || (sizeof(Func_) <= sizeof(pvoid)), 
    functor_type&>::type_t set(Func_ f)
    {
        assign_to(f);
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename Func_>
    typename nx::enable_if<!nx::is_pointer<Func_>::value && (sizeof(Func_) > sizeof(pvoid)), 
    functor_type&>::type_t set(const Func_& f)
    {
        assign_to(guard(f));
        return (*reinterpret_cast<functor_type*>(this));
    }
};

/*
    The functor for saving function and functor
*/

template <typename F>
class functor;

template <typename R>
class functor<R()>
    : public functor_base<R(), functor<R()>, R(*)(private_functor::handler&)>
{
    typedef functor_base<R(), functor<R()>, R(*)(private_functor::handler&)> base_t;
public:
    functor(void)         : base_t() {}
    functor(nx::nulptr_t) : base_t() {}
    functor(nx::none_t)   : base_t() {}
    template <typename Func_>
    functor(const Func_& f, typename nx::enable_if<!is_sametype<Func_, int>::value, int>::type_t = 0)
        : base_t(f)
    {}
    functor(const functor& fr)
        : base_t(static_cast<const base_t&>(fr))
    {}
    functor(const rvalue<functor>& fr)
        : base_t()
    { nx::swap(*this, fr); }
public:
    functor& operator=(const functor& fr)         { return base_t::operator=(fr); }
    functor& operator=(const rvalue<functor>& fr) { return base_t::operator=(fr); }
    using base_t::swap;
public:
    R operator()(void)
    {
        return (*base_t::invoker_)(base_t::handler_);
    }
};

#define NX_FUNCTOR_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class functor<R(NX_PP_TYPE_1(n, P))> \
    : public functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), \
        functor<R(NX_PP_TYPE_1(n, P))>, \
        R(*)(private_functor::handler&, NX_PP_TYPE_1(n, typename nx::traits<P, >::param_t)) \
    > \
{ \
    typedef functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), \
        functor<R(NX_PP_TYPE_1(n, P))>, \
        R(*)(private_functor::handler&, NX_PP_TYPE_1(n, typename nx::traits<P, >::param_t)) \
    > base_t; \
public: \
    functor(void)         : base_t() {} \
    functor(nx::nulptr_t) : base_t() {} \
    functor(nx::none_t)   : base_t() {} \
    template <typename Func_> \
    functor(const Func_& f, typename nx::enable_if<!is_sametype<Func_, int>::value, int>::type_t = 0) \
        : base_t(f) \
    {} \
    functor(const functor& fr) \
        : base_t(static_cast<const base_t&>(fr)) \
    {} \
    functor(const rvalue<functor>& fr) \
        : base_t() \
    { nx::swap(*this, fr); } \
public: \
    functor& operator=(const functor& fr)         { return base_t::operator=(fr); } \
    functor& operator=(const rvalue<functor>& fr) { return base_t::operator=(fr); } \
    using base_t::swap; \
public: \
    R operator()(NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
    { \
        return (*base_t::invoker_)(base_t::handler_, NX_PP_TYPE_1(n, par)); \
    } \
};
NX_PP_MULT_MAX(NX_FUNCTOR_)
#undef NX_FUNCTOR_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
