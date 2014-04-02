/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/pattern/prototype.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_functor
{
    /*
        The handler for storing a function
    */

    typedef nx::empty_t class_t;

    union handler
    {
        void(*fun_ptr)();
        void* obj_ptr;
        struct
        {
            void* this_ptr;
            void(class_t::*func_ptr)();
        } mem_ptr;

        template <typename FuncT>
        static FuncT* cast(pvoid& p)
        {
            /*
                this cast is for disable the gcc warning:
                dereferencing type-punned pointer will break strict-aliasing rules
            */
            return reinterpret_cast<FuncT*>(&(p));
        }
    };

    template <typename T, bool = is_function<T>::value>
    struct check_type;

    template <typename T>
    struct check_type<T, true>
    {
        typedef nx::true_t type_t;
    };

    template <typename T>
    struct check_type<T, false>
    {
        typedef nx::false_t type_t;
    };

    /*
        The invoker for call a function
    */

    template <typename F, typename FuncT, typename ThisT = nx::null_t, bool = is_function<FuncT>::value
                                                                     , bool = is_pointer<FuncT>::value>
    struct invoker;

    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, true, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<FuncT>(hd.fun_ptr)))();
        }
    };

    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, false, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<FuncT>(hd.obj_ptr)))();
        }
    };

    template <typename R, typename FuncT>
    struct invoker<R(), FuncT, nx::null_t, false, false>
    {
        static R invoke(handler& hd)
        {
            return (*(handler::cast<FuncT>(hd.obj_ptr)))(); // hd.obj_ptr is not a pointer
        }
    };

    template <typename R, typename FuncT, typename ThisT>
    struct invoker<R(), FuncT, ThisT, true, true>
    {
        static R invoke(handler& hd)
        {
            return (reinterpret_cast<ThisT>(hd.mem_ptr.this_ptr)->*
                    reinterpret_cast<FuncT>(hd.mem_ptr.func_ptr))();
        }
    };

    // the void()'s return value may not be void

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.fun_ptr)))();
        }
    };

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, false, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<FuncT>(hd.obj_ptr)))();
        }
    };

    template <typename FuncT>
    struct invoker<void(), FuncT, nx::null_t, false, false>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(handler::cast<FuncT>(hd.obj_ptr)))();
        }
    };

    template <typename FuncT, typename ThisT>
    struct invoker<void(), FuncT, ThisT, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (reinterpret_cast<ThisT>(hd.mem_ptr.this_ptr)->*
                        reinterpret_cast<FuncT>(hd.mem_ptr.func_ptr))();
        }
    };

#define NX_FUNCTOR_INVOKER_(n) \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(reinterpret_cast<FuncT>(hd.fun_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(reinterpret_cast<FuncT>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, false> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(handler::cast<FuncT>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename FuncT, typename ThisT> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), FuncT, ThisT, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (reinterpret_cast<ThisT>(hd.mem_ptr.this_ptr)->* \
                    reinterpret_cast<FuncT>(hd.mem_ptr.func_ptr))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(reinterpret_cast<FuncT>(hd.fun_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(reinterpret_cast<FuncT>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, nx::null_t, false, false> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(handler::cast<FuncT>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename FuncT, typename ThisT> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), FuncT, ThisT, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (reinterpret_cast<ThisT>(hd.mem_ptr.this_ptr)->* \
                        reinterpret_cast<FuncT>(hd.mem_ptr.func_ptr))(NX_PP_TYPE_1(n, par)); \
        } \
    };
    NX_PP_MULT_MAX(NX_FUNCTOR_INVOKER_)
#undef NX_FUNCTOR_INVOKER_
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
        virtual size_t size_of(void) = 0;
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
        PlaceHolder* clone(void)  { return nx::alloc<Holder>(nx::ref(held_)); }
        size_t size_of(void)      { return sizeof(Holder); }
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
    {
        nx::initialize(handler_);
    }

    template <typename FuncT>
    functor_base(const FuncT& f)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(f);
    }

    template <typename FuncT, typename ObjT>
    functor_base(FuncT f, ObjT* o)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(f, o);
    }

    functor_base(const functor_base& fr)
        : invoker_(fr.invoker_), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        if (fr.any_guard_)
        {
            any_guard_ = nx::clone(fr.any_guard_);
            // only obj_ptr_tag may make a guard
            handler_.obj_ptr = any_guard_->handler();
        }
        else
            handler_ = fr.handler_;
    }

    ~functor_base(void)
    {
        nx::free(any_guard_);
    }

public:
    bool checkSafeBool(void) const
    {
        return !!invoker_;
    }

    void swap(functor_type& r)
    {
        nx::swap(handler_  , r.handler_);
        nx::swap(invoker_  , r.invoker_);
        nx::swap(any_guard_, r.any_guard_);
    }

    functor_type& operator=(functor_type& fr) // do swap
    {
        fr.swap(*reinterpret_cast<functor_type*>(this));
        return (*reinterpret_cast<functor_type*>(this));
    }

    friend bool operator==(const functor_type& f1, const functor_type& f2)
    {
        return nx::equal(f1.handler_, f2.handler_);
    }

protected:
    template <typename FuncT>
    void assign_to(FuncT f, nx::true_t)
    {
        handler_.fun_ptr = reinterpret_cast<void(*)()>(f);
        invoker_ = &private_functor::invoker<style_type, FuncT>::invoke;
    }

    template <typename FuncT>
    void assign_to(FuncT f, nx::false_t)
    {
        memcpy(&(handler_.obj_ptr), &f, sizeof(pvoid)); // f may be not a pointer
        invoker_ = &private_functor::invoker<style_type, FuncT>::invoke;
    }

    template <typename FuncT, typename ObjT>
    void assign_to(FuncT f, ObjT* o)
    {
        handler_.mem_ptr.this_ptr = reinterpret_cast<void*>(o);
        handler_.mem_ptr.func_ptr = reinterpret_cast<void(private_functor::class_t::*)()>(f);
        invoker_ = &private_functor::invoker<style_type, FuncT, ObjT*>::invoke;
    }

public:
    template <typename FuncT>
    typename nx::enable_if<is_pointer<FuncT>::value || (sizeof(FuncT) <= sizeof(pvoid)),
    functor_type&>::type_t bind(FuncT f)
    {
        assign_to(f, typename private_functor::check_type<FuncT>::type_t());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename FuncT>
    typename nx::enable_if<!is_pointer<FuncT>::value && (sizeof(FuncT) > sizeof(pvoid)),
    functor_type&>::type_t bind(const FuncT& f)
    {
        assign_to(guard(f), nx::false_t());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename FuncT, typename ObjT>
    functor_type& bind(FuncT f, ObjT* o)
    {
        assign_to(f, o);
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
    template <typename FuncT>
    functor(const FuncT& f, typename nx::enable_if<!is_sametype<FuncT, int>::value, int>::type_t = 0)
        : base_t(f)
    {}
    template <typename FuncT, typename ObjT>
    functor(FuncT f, ObjT* o)
        : base_t(f, o)
    {}
    functor(const functor& fr)
        : base_t(static_cast<const base_t&>(fr))
    {}
    functor(const rvalue<functor, true>& fr)
        : base_t()
    { swap(unmove(fr)); }
public:
    functor& operator=(functor fr) { return base_t::operator=(fr); }
    using base_t::swap;
public:
    R operator()(void) const
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
    template <typename FuncT> \
    functor(const FuncT& f, typename nx::enable_if<!is_sametype<FuncT, int>::value, int>::type_t = 0) \
        : base_t(f) \
    {} \
    template <typename FuncT, typename ObjT> \
    functor(FuncT f, ObjT* o) \
        : base_t(f, o) \
    {} \
    functor(const functor& fr) \
        : base_t(static_cast<const base_t&>(fr)) \
    {} \
    functor(const rvalue<functor, true>& fr) \
        : base_t() \
    { swap(unmove(fr)); } \
public: \
    functor& operator=(functor fr) { return base_t::operator=(fr); } \
    using base_t::swap; \
public: \
    R operator()(NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) const \
    { \
        return (*base_t::invoker_)(base_t::handler_, NX_PP_TYPE_1(n, par)); \
    } \
};
NX_PP_MULT_MAX(NX_FUNCTOR_)
#undef NX_FUNCTOR_

/*
    Special swap algorithm
*/

template <typename F>
inline void swap(functor<F>& x, functor<F>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
