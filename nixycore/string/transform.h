/*
    The Nixy Library
    Code covered by the MIT License

    Modified from UnicodeConverter
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2010. Jianhui Qin (http://blog.csdn.net/jhqin)
*/

#pragma once

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace transform
{
    namespace private_
    {
        template <size_t X> struct utf_type;
        template <>         struct utf_type<1> { typedef uint8  type_t; };
        template <>         struct utf_type<2> { typedef uint16 type_t; };
        template <>         struct utf_type<4> { typedef uint32 type_t; };

        template <typename T, size_t X>
        struct check
            : type_if<(sizeof(T) == sizeof(typename utf_type<X>::type_t)) && !is_pointer<T>::value>
        {};
    }

    using namespace transform::private_;

    /*
        UTF-32 to UTF-8
    */

    template <typename T, typename U>
    typename enable_if<check<T, 4>::value && check<U, 1>::value,
    size_t>::type_t utf(T src, U* des)
    {
        if (src == 0) return 0;

        static const byte PREFIX[] = { 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
        static const uint32 CODE_UP[] =
        {
            0x80,           // U+00000000 - U+0000007F
            0x800,          // U+00000080 - U+000007FF
            0x10000,        // U+00000800 - U+0000FFFF
            0x200000,       // U+00010000 - U+001FFFFF
            0x4000000,      // U+00200000 - U+03FFFFFF
            0x80000000      // U+04000000 - U+7FFFFFFF
        };

        size_t i, len = sizeof(CODE_UP) / sizeof(uint32);
        for(i = 0; i < len; ++i)
            if (src < CODE_UP[i]) break;

        if (i == len) return 0; // the src is invalid

        len = i + 1;
        if (des)
        {
            for(; i > 0; --i)
            {
                des[i] = static_cast<U>((src & 0x3F) | 0x80);
                src >>= 6;
            }
            des[0] = static_cast<U>(src | PREFIX[len - 1]);
        }
        return len;
    }

    /*
        UTF-8 to UTF-32
    */

    template <typename T, typename U>
    typename enable_if<check<T, 1>::value && check<U, 4>::value,
    size_t>::type_t utf(const T* src, U& des)
    {
        if (!src || (*src) == 0) return 0;

        uint8 b = *(src++);

        if (b < 0x80)
        {
            des = b;
            return 1;
        }

        if (b < 0xC0 || b > 0xFD) return 0; // the src is invalid

        size_t len;

        if (b < 0xE0)
        {
            des = b & 0x1F;
            len = 2;
        }
        else
        if (b < 0xF0)
        {
            des = b & 0x0F;
            len = 3;
        }
        else
        if (b < 0xF8)
        {
            des = b & 0x07;
            len = 4;
        }
        else
        if (b < 0xFC)
        {
            des = b & 0x03;
            len = 5;
        }
        else
        {
            des = b & 0x01;
            len = 6;
        }

        size_t i = 1;
        for (; i < len; ++i)
        {
            b = *(src++);
            if (b < 0x80 || b > 0xBF) return 0; // the src is invalid
            des = (des << 6) + (b & 0x3F);
        }
        return len;
    }

    /*
        UTF-32 to UTF-16
    */

    template <typename T, typename U>
    typename enable_if<check<T, 4>::value && check<U, 2>::value,
    size_t>::type_t utf(T src, U* des)
    {
        if (src == 0) return 0;

        if (src <= 0xFFFF)
        {
            if (des) (*des) = static_cast<U>(src);
            return 1;
        }
        else
        if (src <= 0xEFFFF)
        {
            if (des)
            {
                des[0] = static_cast<U>(0xD800 + (src >> 10) - 0x40);  // high
                des[1] = static_cast<U>(0xDC00 + (src & 0x03FF));      // low
            }
            return 2;
        }
        return 0;
    }

    /*
        UTF-16 to UTF-32
    */

    template <typename T, typename U>
    typename enable_if<check<T, 2>::value && check<U, 4>::value,
    size_t>::type_t utf(const T* src, U& des)
    {
        if (!src || (*src) == 0) return 0;

        uint16 w1 = src[0];
        if (w1 >= 0xD800 && w1 <= 0xDFFF)
        {
            if (w1 < 0xDC00)
            {
                uint16 w2 = src[1];
                if (w2 >= 0xDC00 && w2 <= 0xDFFF)
                {
                    des = (w2 & 0x03FF) + (((w1 & 0x03FF) + 0x40) << 10);
                    return 2;
                }
            }
            return 0; // the src is invalid
        }
        else
        {
            des = w1;
            return 1;
        }
    }

    /*
        UTF-16 to UTF-8
    */

    template <typename T, typename U>
    typename enable_if<check<T, 2>::value && check<U, 1>::value,
    size_t>::type_t utf(T src, U* des)
    {
        // make utf-16 to utf-32
        uint32 tmp;
        if (utf(&src, tmp) != 1) return 0;
        // make utf-32 to utf-8
        return utf(tmp, des);
    }

    /*
        UTF-8 to UTF-16
    */

    template <typename T, typename U>
    typename enable_if<check<T, 1>::value && check<U, 2>::value,
    size_t>::type_t utf(const T* src, U& des)
    {
        // make utf-8 to utf-32
        uint32 tmp;
        size_t len = utf(src, tmp);
        if (len == 0) return 0;
        // make utf-32 to utf-16
        if (utf(tmp, &des) != 1) return 0;
        return len;
    }

    /*
        UTF-X: string to string
    */

    template <typename T, typename U>
    typename enable_if<check<T, 4>::value && (check<U, 1>::value || check<U, 2>::value),
    size_t>::type_t utf(const T* src, U* des)   // UTF-32 to UTF-X(8/16)
    {
        if (!src || (*src) == 0) return 0;

        size_t num = 0;
        for(; *src; ++src)
        {
            size_t len = utf(*src, des);
            if (len == 0) break;
            if (des) des += len;
            num += len;
        }
        if (des) (*des) = 0;
        return num;
    }

    template <typename T, typename U>
    typename enable_if<(check<T, 1>::value || check<T, 2>::value) && check<U, 4>::value,
    size_t>::type_t utf(const T* src, U* des)   // UTF-X(8/16) to UTF-32
    {
        if (!src || (*src) == 0) return 0;

        size_t num = 0;
        while(*src)
        {
            uint32 tmp;
            size_t len = utf(src, tmp);
            if (len == 0) break;
            if (des)
            {
                (*des) = tmp;
                ++des;
            }
            src += len;
            num += 1;
        }
        if (des) (*des) = 0;
        return num;
    }

    template <typename T, typename U>
    typename enable_if<(check<T, 1>::value && check<U, 2>::value) ||
                       (check<T, 2>::value && check<U, 1>::value),
    size_t>::type_t utf(const T* src, U* des)    // UTF-X(8/16) to UTF-Y(16/8)
    {
        if (!src || (*src) == 0) return 0;

        size_t num = 0;
        while(*src)
        {
            // make utf-x to utf-32
            uint32 tmp;
            size_t len = utf(src, tmp);
            if (len == 0) break;
            src += len;
            // make utf-32 to utf-y
            len = utf(tmp, des);
            if (len == 0) break;
            if (des) des += len;
            num += len;
        }
        if (des) (*des) = 0;
        return num;
    }
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
