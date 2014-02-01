/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "utility/safebool.h"
#include "utility/refer.h"
#include "utility/final.h"

#include "general/general.h"
#include "typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_valid
{
    template <typename T, bool = is_reference<T>::value>
    class detail;

    template <typename T>
    class detail<T, true> : public SafeBool<detail<T, true> >
    {
    public:
        typedef typename rm_const
                    <typename rm_reference
                        <T
                        >::type_t
                    >::type_t type_t;
        typedef typename rm_reference
                    <T
                    >::type_t value_t;

    public:
        value_t* value_;

    public:
        detail(void)       : value_(nx::nulptr) {}
        detail(nx::none_t) : value_(nx::nulptr) {}
        detail(value_t& r) : value_(nx::nulptr) { (*this) = r; }

        detail& operator=(value_t& r)
        {
            value_ = &r;
            return (*this);
        }

        void reset(void)
        {
            value_ = nx::nulptr;
        }

        bool checkSafeBool(void) const { return !!value_; }

        value_t&      operator* (void)       { return *value_; }
        const type_t& operator* (void) const { return *value_; }
        value_t*      operator->(void)       { return value_; }
        const type_t* operator->(void) const { return value_; }
    };

    template <typename T>
    class detail<T, false> : public SafeBool<detail<T, false> >
    {
    public:
        typedef typename rm_const<T>::type_t type_t;
        typedef T value_t;

    public:
        bool is_valid_;
        value_t value_;

    public:
        detail(void)            : is_valid_(false) {}
        detail(nx::none_t)      : is_valid_(false) {}
        detail(const type_t& r) : is_valid_(false)
                                , value_(r) {}

        detail& operator=(const type_t& r)
        {
            value_ = r;
            is_valid_ = true;
            return (*this);
        }

        void reset(void)
        {
            is_valid_ = false;
        }

        bool checkSafeBool(void) const { return is_valid_; }

        value_t&      operator* (void)       { return value_; }
        const type_t& operator* (void) const { return value_; }
        value_t*      operator->(void)       { return &value_; }
        const type_t* operator->(void) const { return &value_; }
    };
}

/*
    valid for clearly define invalid value
*/

template <typename T>
class valid : public private_valid::detail<T>, Final<valid<T> >
{
    typedef private_valid::detail<T> base_t;

public:
    valid(void)         : base_t()  {}
    valid(nx::none_t n) : base_t(n) {}
    valid(const typename base_t::type_t& r)
        : base_t(r)
    {}

    template <typename U>
    valid(valid<U>& r) { (*this) = r; }

public:
    template <typename U>
    valid& operator=(valid<U>& r)
    {
        if (r) { (*this) = (*r); }
        else   { base_t::reset(); }
        return (*this);
    }
    using base_t::operator=;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
