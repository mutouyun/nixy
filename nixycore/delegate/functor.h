/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/delegate/function_traits.h"

#include "nixycore/memory/default_alloc.h"
#include "nixycore/pattern/prototype.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_functor
{
    /*
        Some helper templates
    */

    template <typename T>
    struct check_type
        : type_if<is_function<typename nx::decay<T>::type_t>::value>
    {};

    template <typename T>
    struct check_size
        : type_if<sizeof(typename nx::decay<T>::type_t) <= sizeof(pvoid)>
    {};

    /*
        The handler for storing a function
    */

    typedef nx::empty_t class_t;

    union handler
    {
        void(*fun_ptr_)();
        void* obj_ptr_;
        struct
        {
            void* this_ptr_;
            void (class_t::*func_ptr_)();
        } mem_ptr_;

        template <typename FuncT>
        static FuncT* cast(pvoid& p)
        {
            /*
                <GNUC> compiler warning:
                dereferencing type-punned pointer will break strict-aliasing rules
            */
            return reinterpret_cast<FuncT*>(&(p));
        }
    };

    /*
        The invoker for call a function
    */

    template <typename F, typename FuncT, typename ThisT = nx::null_t, bool = is_function<FuncT>::value
                                                                     , bool = is_pointer<FuncT>::value>
    struct invoker;

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename R, typename... P, typename FuncT>
    struct invoker<R(P...), FuncT, nx::null_t, true, true>
    {
        static R invoke(handler& hd, P... par)
        {
            return (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename R, typename... P, typename FuncT>
    struct invoker<R(P...), FuncT, nx::null_t, false, true>
    {
        static R invoke(handler& hd, P... par)
        {
            return (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename R, typename... P, typename FuncT>
    struct invoker<R(P...), FuncT, nx::null_t, false, false>
    {
        static R invoke(handler& hd, P... par)
        {
            return (*(handler::cast<FuncT>(hd.obj_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename R, typename... P, typename FuncT, typename ThisT>
    struct invoker<R(P...), FuncT, ThisT, true, true>
    {
        static R invoke(handler& hd, P... par)
        {
            return (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->*
                    reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))(nx_forward(P, par)...);
        }
    };

    // the void()'s return value may not be void

    template <typename... P, typename FuncT>
    struct invoker<void(P...), FuncT, nx::null_t, true, true>
    {
        static void invoke(handler& hd, P... par)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename... P, typename FuncT>
    struct invoker<void(P...), FuncT, nx::null_t, false, true>
    {
        static void invoke(handler& hd, P... par)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename... P, typename FuncT>
    struct invoker<void(P...), FuncT, nx::null_t, false, false>
    {
        static void invoke(handler& hd, P... par)
        {
            /*return*/ (*(handler::cast<FuncT>(hd.obj_ptr_)))(nx_forward(P, par)...);
        }
    };

    template <typename... P, typename FuncT, typename ThisT>
    struct invoker<void(P...), FuncT, ThisT, true, true>
    {
        static void invoke(handler& hd, P... par)
        {
            /*return*/ (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->*
                        reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))(nx_forward(P, par)...);
        }
    };
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, true, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))();
        }
    };

    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, false, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))();
        }
    };

    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, false, false>
    {
        static R invoke(handler& hd)
        {
            return (*(handler::cast<FuncT>(hd.obj_ptr_)))(); // hd.obj_ptr_ is not a pointer
        }
    };

    template <typename R, typename FuncT, typename ThisT>
    struct invoker<R(), FuncT, ThisT, true, true>
    {
        static R invoke(handler& hd)
        {
            return (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->*
                    reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))();
        }
    };

    // the void()'s return value may not be void

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))();
        }
    };

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, false, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))();
        }
    };

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, false, false>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(handler::cast<FuncT>(hd.obj_ptr_)))();
        }
    };

    template <typename FuncT, typename ThisT>
    struct invoker<void(), FuncT, ThisT, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->*
                        reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))();
        }
    };

#define NX_FUNCTOR_INVOKER_(n) \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            return (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            return (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, false> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            return (*(handler::cast<FuncT>(hd.obj_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT, typename ThisT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, ThisT, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            return (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->* \
                    reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            /*return*/ (*(reinterpret_cast<FuncT>(hd.fun_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            /*return*/ (*(reinterpret_cast<FuncT>(hd.obj_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, false> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            /*return*/ (*(handler::cast<FuncT>(hd.obj_ptr_)))(NX_PP_FORWARD(n, P, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT, typename ThisT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, ThisT, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, P, par)) \
        { \
            /*return*/ (reinterpret_cast<ThisT>(hd.mem_ptr_.this_ptr_)->* \
                        reinterpret_cast<FuncT>(hd.mem_ptr_.func_ptr_))(NX_PP_FORWARD(n, P, par)); \
        } \
    };
    NX_PP_MULT_MAX(NX_FUNCTOR_INVOKER_)
#undef NX_FUNCTOR_INVOKER_
#endif/*NX_SP_CXX11_TEMPLATES*/
}

/*
    The base class of functor
*/

template <typename StyleT, typename FunctorT, typename InvokerT>
class functor_base : public safe_bool<functor_base<StyleT, FunctorT, InvokerT> >
                   , public unequal<FunctorT>
{
public:
    typedef StyleT   style_type;
    typedef FunctorT functor_type;
    typedef InvokerT invoker_type;

protected:
    mutable private_functor::handler handler_;
    mutable invoker_type             invoker_;

private:
    class PlaceHolder
    {
    public:
        virtual ~PlaceHolder(void) {}

    public:
        virtual pvoid handler(void) const = 0;
        virtual PlaceHolder* clone(void) = 0;
        virtual size_t size_of(void) const = 0;
    } * any_guard_;

    template <typename T, bool = nx::is_copyable<T>::value>
    class HolderBase : public PlaceHolder
    {
    protected:
        T held_;

    public:
        template <typename U>
        HolderBase(nx_fref(U) v)
            : held_(nx::unref(nx_extract(U, v)))
        {}
    };

    template <typename T>
    class HolderBase<T, false> : public PlaceHolder
    {
    protected:
        T held_;

    public:
        template <typename U>
        HolderBase(nx_fref(U) v)
            : held_(nx::move(nx::unref(nx_extract(U, v))))
        {}
    };

    template <typename T>
    class Holder : public HolderBase<T>
    {
        using HolderBase<T>::held_;

    public:
        template <typename U>
        Holder(nx_fref(U) v)
            : HolderBase<T>(nx_forward(U, v))
        {}

    public:
        pvoid handler(void) const  { return (pvoid)nx::addressof(held_); }
        PlaceHolder* clone(void)   { return nx::alloc<Holder>(nx::ref(held_)); }
        size_t size_of(void) const { return sizeof(Holder); }
    };

protected:
    template <typename FuncT>
    typename nx::decay<FuncT>::type_t* guard(nx_fref(FuncT) f)
    {
        typedef typename nx::decay<FuncT>::type_t func_t;
        any_guard_ = nx::alloc<Holder<func_t> >(nx_forward(FuncT, f));
        return (func_t*)(any_guard_->handler());
    }

public:
    functor_base(void)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
    }

    template <typename FuncT>
    functor_base(nx_fref(FuncT) f,
                 typename enable_if_diff<FuncT, functor_base, int>::type_t = 0)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(nx_forward(FuncT, f));
    }

    template <typename FuncT, typename ObjT>
    functor_base(nx_fref(FuncT) f, ObjT* o)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(nx_forward(FuncT, f), o);
    }

    functor_base(const functor_base& fr)
        : invoker_(fr.invoker_), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        if (fr.any_guard_)
        {
            any_guard_ = nx::clone(fr.any_guard_);
            // only obj_ptr_tag may make a guard
            handler_.obj_ptr_ = any_guard_->handler();
        }
        else
            handler_ = fr.handler_;
    }

    ~functor_base(void)
    {
        nx::free(any_guard_);
    }

public:
    bool check_safe_bool(void) const
    {
        return !!invoker_;
    }

    void swap(functor_type& r)
    {
        nx::swap(handler_  , r.handler_);
        nx::swap(invoker_  , r.invoker_);
        nx::swap(any_guard_, r.any_guard_);
    }

    friend bool operator==(const functor_type& f1, const functor_type& f2)
    {
        return nx::equal(f1.handler_, f2.handler_);
    }

private:
    // function

    template <typename FuncT>
    void assign_to(nx_fref(FuncT) f, nx::true_t)
    {
        typedef typename nx::decay<FuncT>::type_t func_t;
        handler_.fun_ptr_ = reinterpret_cast<void(*)()>(nx_extract(FuncT, f));
        invoker_ = &private_functor::invoker<style_type, func_t>::invoke;
    }

    // pointer, or function object ( small than sizeof(void*) )

    template <typename FuncT>
    void assign_to(nx_fref(FuncT) f, nx::false_t,
                   typename enable_if<nx::is_copyable<typename nx::decay<FuncT>::type_t>::value, int>::type_t = 0)
    {
        typedef typename nx::decay<FuncT>::type_t func_t;
        nx_construct( reinterpret_cast<func_t*>(&(handler_.obj_ptr_)), func_t, (nx_extract(FuncT, f)) );
        invoker_ = &private_functor::invoker<style_type, func_t>::invoke;
    }

    template <typename FuncT>
    void assign_to(nx_fref(FuncT) f, nx::false_t,
                   typename enable_if<!nx::is_copyable<typename nx::decay<FuncT>::type_t>::value, int>::type_t = 0)
    {
        typedef typename nx::decay<FuncT>::type_t func_t;
        nx_construct( reinterpret_cast<func_t*>(&(handler_.obj_ptr_)), func_t, (nx::move(nx_extract(FuncT, f))) );
        invoker_ = &private_functor::invoker<style_type, func_t>::invoke;
    }

    // member function

    template <typename FuncT, typename ObjT>
    void assign_to(nx_fref(FuncT) f, ObjT* o)
    {
        typedef typename nx::decay<FuncT>::type_t func_t;
        handler_.mem_ptr_.this_ptr_ = reinterpret_cast<void*>(o);
        handler_.mem_ptr_.func_ptr_ = reinterpret_cast<void(private_functor::class_t::*)()>(nx_extract(FuncT, f));
        invoker_ = &private_functor::invoker<style_type, func_t, ObjT*>::invoke;
    }

public:
    template <typename FuncT>
    typename nx::enable_if<private_functor::check_size<FuncT>::value,
    functor_type&>::type_t bind(nx_fref(FuncT) f)
    {
        assign_to(nx_forward(FuncT, f), typename private_functor::check_type<FuncT>::type_t());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename FuncT>
    typename nx::enable_if<!private_functor::check_size<FuncT>::value,
    functor_type&>::type_t bind(nx_fref(FuncT) f)
    {
        assign_to(guard(nx_forward(FuncT, f)), nx::false_t());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename FuncT, typename ObjT>
    functor_type& bind(nx_fref(FuncT) f, ObjT* o)
    {
        assign_to(nx_forward(FuncT, f), o);
        return (*reinterpret_cast<functor_type*>(this));
    }
};

/*
    Class that can wrap any kind of callable element
    (such as functions and function objects) into a copyable object,
    and whose type depends solely on its call signature
    (and not on the callable element type itself).
*/

template <typename F>
class functor;

#ifdef NX_SP_CXX11_TEMPLATES
template <typename R, typename... P>
class functor<R(P...)>
    : public functor_base<R(P...), functor<R(P...)>, R(*)(private_functor::handler&, P...)>
{
    typedef functor_base<R(P...), functor<R(P...)>, R(*)(private_functor::handler&, P...)> base_t;

public:
    functor(void)         : base_t() {}
    functor(nx::nulptr_t) : base_t() {}
    functor(nx::none_t)   : base_t() {}

    template <typename FuncT>
    functor(nx_fref(FuncT) /*f*/,
            typename enable_if_same<FuncT, int, int>::type_t = 0)
        : base_t()
    {}

    template <typename FuncT>
    functor(nx_fref(FuncT) f,
            typename enable_if_diff<FuncT, int, int>::type_t = 0,
            typename enable_if_diff<FuncT, functor, int>::type_t = 0)
        : base_t(nx_forward(FuncT, f))
    {}

    template <typename FuncT, typename ObjT>
    functor(nx_fref(FuncT) f, ObjT* o)
        : base_t(nx_forward(FuncT, f), o)
    {}

    functor(const functor& fr)
        : base_t(static_cast<const base_t&>(fr))
    {}

    functor(nx_rref(functor, true) fr)
        : base_t()
    {
        swap(nx::moved(fr));
    }

public:
    using base_t::swap;

    functor& operator=(functor fr)
    {
        fr.swap(*this);
        return (*this);
    }

public:
    template <typename... P_>
    R operator()(nx_fref(P_)... par) const
    {
        return (*base_t::invoker_)(base_t::handler_, nx_forward(P_, par)...);
    }
};
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename R>
class functor<R()>
    : public functor_base<R(), functor<R()>, R(*)(private_functor::handler&)>
{
    typedef functor_base<R(), functor<R()>, R(*)(private_functor::handler&)> base_t;

public:
    functor(void)         : base_t() {}
    functor(nx::nulptr_t) : base_t() {}
    functor(nx::none_t)   : base_t() {}

    template <typename FuncT>
    functor(nx_fref(FuncT) /*f*/,
            typename enable_if_same<FuncT, int, int>::type_t = 0)
        : base_t()
    {}

    template <typename FuncT>
    functor(nx_fref(FuncT) f,
            typename enable_if_diff<FuncT, int, int>::type_t = 0,
            typename enable_if_diff<FuncT, functor, int>::type_t = 0)
        : base_t(nx_forward(FuncT, f))
    {}

    template <typename FuncT, typename ObjT>
    functor(nx_fref(FuncT) f, ObjT* o)
        : base_t(nx_forward(FuncT, f), o)
    {}

    functor(const functor& fr)
        : base_t(static_cast<const base_t&>(fr))
    {}

    functor(nx_rref(functor, true) fr)
        : base_t()
    {
        swap(nx::moved(fr));
    }

public:
    using base_t::swap;

    functor& operator=(functor fr)
    {
        fr.swap(*this);
        return (*this);
    }

public:
    R operator()(void) const
    {
        return (*base_t::invoker_)(base_t::handler_);
    }
};

#define NX_FUNCTOR_DEFINE_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class functor<R(NX_PP_TYPE_1(n, P))> \
    : public functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), functor<R(NX_PP_TYPE_1(n, P))>, \
        R (*)(private_functor::handler&, NX_PP_TYPE_1(n, P)) \
    > \
{ \
    typedef functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), functor<R(NX_PP_TYPE_1(n, P))>, \
        R (*)(private_functor::handler&, NX_PP_TYPE_1(n, P)) \
    > base_t; \
public: \
    functor(void)         : base_t() {} \
    functor(nx::nulptr_t) : base_t() {} \
    functor(nx::none_t)   : base_t() {} \
    template <typename FuncT> \
    functor(FuncT NX_PF_SYM_, \
            typename enable_if_same<FuncT, int, int>::type_t = 0) \
        : base_t() \
    {} \
    template <typename FuncT> \
    functor(nx_fref(FuncT) f, \
            typename enable_if_diff<FuncT, int, int>::type_t = 0, \
            typename enable_if_diff<FuncT, functor, int>::type_t = 0) \
        : base_t(nx_forward(FuncT, f)) \
    {} \
    template <typename FuncT, typename ObjT> \
    functor(nx_fref(FuncT) f, ObjT* o) \
        : base_t(nx_forward(FuncT, f), o) \
    {} \
    functor(const functor& fr) \
        : base_t(static_cast<const base_t&>(fr)) \
    {} \
    functor(nx_rref(functor, true) fr) \
        : base_t() \
    { \
        swap(nx::moved(fr)); \
    } \
public: \
    using base_t::swap; \
    functor& operator=(functor fr) \
    { \
        fr.swap(*this); \
        return (*this); \
    } \
public: \
    template <NX_PP_TYPE_1(n, typename P_)> \
    R operator()(NX_PP_TYPE_2(n, P_, NX_PP_FREF(par))) const \
    { \
        return (*base_t::invoker_)(base_t::handler_, NX_PP_FORWARD(n, P_, par)); \
    } \
};
NX_PP_MULT_MAX(NX_FUNCTOR_DEFINE_)
#undef NX_FUNCTOR_DEFINE_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    Special swap algorithm
*/

template <typename F>
inline void swap(functor<F>& x, functor<F>& y)
{
    x.swap(y);
}

/*
    Bind member function and the object pointer to a functor
*/

template <typename T, typename C, typename P>
inline nx_rval(functor<typename function_traits<T C::*>::type_t>, true)
    bind(T C::* f, P* p)
{
    return functor<typename function_traits<T C::*>::type_t>(f, p);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
