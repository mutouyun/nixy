/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/algorithm/container.h"

#include "nixycore/pattern/stream.h"

#include "nixycore/typemanip/typetools.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_assign
{
    template <typename T>
    struct policy_pod
    {
        T* setptr_;

        void operator()(T& set)
        {
            setptr_ = &set;
        }

        template <typename V>
        void operator<<(const V& val)
        {
            *setptr_ = val;
        }

        template <typename V>
        void operator>>(V& val)
        {
            val = *setptr_;
        }
    };

    template <typename T>
    struct policy_array : policy_pod<T>
    {
        typedef policy_pod<T> base_t;

        typename container_traits<T>::ite_t iter_;

        void operator()(T& set)
        {
            base_t::operator()(set);
            iter_ = nx::begin(set);
        }

        template <typename V>
        void operator<<(const V& val)
        {
            if (iter_ != nx::end(*base_t::setptr_))
                nx::insert(*base_t::setptr_, iter_++, val);
        }

        template <typename V>
        void operator>>(V& val)
        {
            if (iter_ != nx::end(*base_t::setptr_))
                val = *(iter_++);
        }
    };

    template <typename T>
    struct policy_container : policy_array<T>
    {
        typedef policy_pod<T> base_t;

        template <typename V>
        void operator<<(const V& val)
        {
            nx::insert(*base_t::setptr_, nx::end(*base_t::setptr_), val);
        }
    };

    template <typename T>
    struct policy_type
    {
        typedef typename select_if
                <
                    is_array<T>::value, 
                    policy_array<T>, 
                    typename select_if
                    <
                        is_container<T>::value, 
                        policy_container<T>, 
                        policy_pod<T>
                    >::type_t
                >::type_t type_t;
    };

    template <typename T>
    struct detail : Stream<typename policy_type<T>::type_t>
    {
        const detail& operator()(T& set)
        {
            Stream<typename policy_type<T>::type_t>::operator()(set);
            return (*this);
        }

        template <typename K, typename V>
        const detail& operator()(const K& k, const V& v) const
        {
            (*this) << typename container_traits<T>::pair_t(k, v);
            return (*this);
        }
    };
}

template <typename T>
struct assign
    : private_assign::detail<T>
{};

#define nx_assign(set, ...) \
    nx::assign<nx_typeof(set)>()(set),##__VA_ARGS__

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
