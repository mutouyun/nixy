/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/utility/safe_bool.h"
#include "nixycore/utility/rvalue.h"
#include "nixycore/utility/alignof.h"

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typebehavior.h"
#include "nixycore/memory/construct.h"
#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_valid
{
    /*
        The storage of valid

        When you initialize it to store the null state,
        it does not construct any T at all.
    */

    template <typename T>
    struct storage
    {
        typename nx::aligned<T>::storage_t value_;

        T*       operator& (void)       { return reinterpret_cast<T*>      (&value_); }
        const T* operator& (void) const { return reinterpret_cast<const T*>(&value_); }

        T*       operator->(void)       { return reinterpret_cast<T*>      (&value_); }
        const T* operator->(void) const { return reinterpret_cast<const T*>(&value_); }
    };

    /*
        The detail of valid
    */

    template <typename T, bool = is_reference<T>::value>
    class detail;

    template <typename T>
    class detail<T, true> : public safe_bool<detail<T, true> >
    {
    public:
        typedef typename rm_reference<T>::type_t   value_t;
        typedef typename rm_const<value_t>::type_t type_t;
        typedef value_t& param_t;

    protected:
        value_t* value_;

    public:
        detail(void)      : value_(nx::nulptr) {}
        detail(param_t r) : value_(nx::nulptr) { set(r); }

        bool is_initialized(void) const
        {
            return !!value_;
        }

        void set(param_t r)
        {
            value_ = &r;
        }

        void reset(void)
        {
            value_ = nx::nulptr;
        }

        template <typename U>
        void swap(detail<U>& rhs)
        {
            nx::swap(value_, rhs.value_);
        }

        bool check_safe_bool(void) const { return is_initialized(); }

        value_t&      operator* (void)       { return *value_; }
        const type_t& operator* (void) const { return *value_; }
        value_t*      operator->(void)       { return  value_; }
        const type_t* operator->(void) const { return  value_; }
    };

    template <typename T>
    class detail<T, false> : public safe_bool<detail<T, false> >
    {
    public:
        typedef T value_t;
        typedef typename rm_const<value_t>::type_t type_t;
        typedef const type_t& param_t;

    protected:
        bool is_valid_;
        storage<value_t> value_;

        void construct(param_t r)
        {
            nx_construct(&(value_), value_t, (r));
        }

        void destruct(void)
        {
            nx_destruct(&(value_), value_t);
        }

    public:
        detail(void)      : is_valid_(false) {}
        detail(param_t r) : is_valid_(false) { set(r); }
        ~detail(void) { reset(); }

        bool is_initialized(void) const
        {
            return is_valid_;
        }

        void set(param_t r)
        {
            if (is_initialized())
            {
                (*(&(value_))) = r;
            }
            else
            {
                construct(r);
                is_valid_ = true;
            }
        }

        void reset(void)
        {
            if (is_initialized())
            {
                is_valid_ = false;
                destruct();
            }
        }

        template <typename U>
        void swap(detail<U>& rhs)
        {
            if (!is_initialized() && !rhs.is_initialized())
            {
                return; // Do nothing
            }
            else
            if (!is_initialized() && rhs.is_initialized())
            {
                set(*rhs);
                rhs.reset();
            }
            else
            if (is_initialized() && !rhs.is_initialized())
            {
                rhs.set(operator*());
                reset();
            }
            else
            {
                nx::swap(value_.value_, rhs.value_.value_);
                return;
            }
            nx::swap(is_valid_, rhs.is_valid_);
        }

        bool check_safe_bool(void) const { return is_initialized(); }

        value_t&      operator* (void)       { return (*(&(value_))); }
        const type_t& operator* (void) const { return (*(&(value_))); }
        value_t*      operator->(void)       { return   (&(value_)) ; }
        const type_t* operator->(void) const { return   (&(value_)) ; }
    };
}

/*
    valid for clearly define invalid value
*/

template <typename T>
class valid : public private_valid::detail<T>
{
    typedef private_valid::detail<T> base_t;

public:
    typedef typename base_t::value_t value_t;
    typedef typename base_t::type_t  type_t;
    typedef typename base_t::param_t param_t;

    valid(void)       : base_t() {}
    valid(nx::none_t) : base_t() {}

    valid(param_t rhs)
        : base_t(rhs)
    {}

    valid(const valid& rhs)
        : base_t()
    {
        set(rhs);
    }

    template <typename U>
    valid(const valid<U>& rhs)
        : base_t()
    {
        set(rhs);
    }

    valid(nx_rref(valid, true) rhs)
    {
        swap(nx::moved(rhs));
    }

public:
    using base_t::set;
    using base_t::reset;
    using base_t::swap;

    template <typename U>
    void set(const valid<U>& rhs)
    {
        if (rhs) base_t::set(const_cast<param_t>(*rhs));
        else     base_t::reset();
    }

    valid& operator=(nx::none_t)
    {
        reset();
        return (*this);
    }

    valid& operator=(param_t rhs)
    {
        base_t::set(rhs);
        return (*this);
    }

    valid& operator=(const valid& rhs)
    {
        set(rhs);
        return (*this);
    }

    template <typename U>
    valid& operator=(const valid<U>& rhs)
    {
        set(rhs);
        return (*this);
    }

    valid& operator=(nx_rref(valid, true) rhs)
    {
        swap(nx::moved(rhs));
        return (*this);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
