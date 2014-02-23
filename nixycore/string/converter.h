/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/string/transform.h"

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    UTF-X to UTF-Y
*/

namespace private_converter
{
    template <size_t X> struct utf_type;
    template <>         struct utf_type<1> { typedef uint8  type_t; };
    template <>         struct utf_type<2> { typedef uint16 type_t; };
    template <>         struct utf_type<4> { typedef uint32 type_t; };

    template <size_t X, typename T>
    struct check
        : type_if<(sizeof(T) == sizeof(typename utf_type<X>::type_t)) &&
                  !is_pointer<T>::value>
    {};

    template <size_t X, size_t Y, bool = (X > Y), bool = (X != Y)>
    struct detail;

    /*
        UTF-X(32/16) to UTF-Y(16/8)
    */

    template <size_t X, size_t Y>
    struct detail<X, Y, true, true>
    {
        typedef typename utf_type<X>::type_t src_t;
        typedef typename utf_type<Y>::type_t des_t;

        template <typename T, typename U>
        static typename enable_if<check<X, T>::value && check<Y, U>::value,
        size_t>::type_t utf(T src, U* des)
        {
            return transform::utf((src_t)(src), (des_t*)(des));
        }

        template <typename T>
        static typename enable_if<check<X, T>::value,
        size_t>::type_t utf(T src)
        {
            return transform::utf((src_t)(src), (des_t*)(0));
        }

        template <typename T, typename U>
        static typename enable_if<check<X, T>::value && check<Y, U>::value,
        size_t>::type_t utf(const T* src, U* des)
        {
            return transform::utf((const src_t*)(src), (des_t*)(des));
        }

        template <typename T>
        static typename enable_if<check<X, T>::value,
        size_t>::type_t utf(const T* src)
        {
            return transform::utf((src_t)(src), (des_t*)(0));
        }
    };

    /*
        UTF-X(16/8) to UTF-Y(32/16)
    */

    template <size_t X, size_t Y>
    struct detail<X, Y, false, true>
    {
        typedef typename utf_type<X>::type_t src_t;
        typedef typename utf_type<Y>::type_t des_t;

        template <typename T, typename U>
        static typename enable_if<check<X, T>::value && check<Y, U>::value,
        size_t>::type_t utf(const T* src, U& des)
        {
            des_t tmp; // for disable the warning strict-aliasing from gcc 4.4
            size_t ret = transform::utf((const src_t*)(src), tmp);
            des = tmp;
            return ret;
        }

        template <typename T, typename U>
        static typename enable_if<check<X, T>::value && check<Y, U>::value,
        size_t>::type_t utf(const T* src, U* des)
        {
            return transform::utf((const src_t*)(src), (des_t*)(des));
        }

        template <typename T>
        static typename enable_if<check<X, T>::value,
        size_t>::type_t utf(const T* src)
        {
            return transform::utf((const src_t*)(src), (des_t*)(0));
        }
    };
}

template <typename T, typename U>
struct converter
    : private_converter::detail<sizeof(T), sizeof(U)>
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
