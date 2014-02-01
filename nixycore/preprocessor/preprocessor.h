/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "preprocessor/ppcount.h"
#include "preprocessor/pparg.h"
#include "preprocessor/pprepeat.h"
#include "preprocessor/ppnest.h"
#include "preprocessor/ppmult.h"

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_CLONE(6, c, ,)
    -->
    c , c , c , c , c , c
*/

#define NX_PP_CLONE_1(n, con, ...)          con
#define NX_PP_CLONE_2(n, con, ...)          __VA_ARGS__ con
#define NX_PP_CLONE(n, con, ...)            NX_PP_REPEATEX(n, NX_PP_CLONE_1, NX_PP_CLONE_2, con, __VA_ARGS__)
#define NX_PP_CLONE_MAX(con, ...)           NX_PP_CLONE(NX_PP_MAX, con, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_REV(1, 2, 3, 4, 5, 6, 7, 8, 9)
    -->
    9, 8, 7, 6, 5, 4, 3, 2, 1
*/

#define NX_PP_REV_(data, ...)               __VA_ARGS__, NX_PP_A1(data)
#define NX_PP_REV(...)                      NX_PP_NEST(NX_PP_COUNT(__VA_ARGS__), NX_PP_REV_, NX_PP_VA, NX_PP_B1, __VA_ARGS__)

/*
    NX_PP_LIMIT(3, a, b, c, d, e, f)
    -->
    a, b, c
*/

#define NX_PP_LIMIT_(data, ...)             NX_PP_A1(data), __VA_ARGS__
#define NX_PP_LIMIT(n, ...)                 NX_PP_NEST(n, NX_PP_LIMIT_, NX_PP_A1, NX_PP_B1, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_ADD(3, 4)
    -->
    7

    NX_PP_SUB(5, 3)
    -->
    2
*/

#define NX_PP_INC(num)                      NX_PP_COUNT(NX_PP_CLONE(num, -, ,) , -)
#define NX_PP_DEC(num)                      NX_PP_COUNT(NX_PP_B1(NX_PP_CLONE(num, -, ,)))

#define NX_PP_ADD(num1, num2)               NX_PP_COUNT(NX_PP_CLONE(num1, -, ,) , NX_PP_CLONE(num2, -, ,))
#define NX_PP_SUB(num1, num2)               NX_PP_COUNT(NX_PP_B(num2, NX_PP_CLONE(num1, -, ,)))

#define NX_PP_REM(num)                      NX_PP_SUB(NX_PP_MAX, num)

/*
    NX_PP_LESS(3, 4, Yes, No)
    -->
    Yes

    NX_PP_MORE(3, 4, Yes, No)
    -->
    No

    NX_PP_EQUA(3, 4, Yes, No)
    -->
    No
*/

#define NX_PP_LESS_NUMBER(num, y, n)        NX_PP_REV(NX_PP_CLONE(num, n, ,) , NX_PP_CLONE(NX_PP_REM(num), y, ,))
#define NX_PP_MORE_NUMBER(num, y, n)        NX_PP_REV(NX_PP_CLONE(NX_PP_DEC(num), y, ,) , NX_PP_CLONE(NX_PP_REM(num), n, ,) , n)

#define NX_PP_LESS(num1, num2, y, n)        NX_PP_HELPER(NX_PP_CLONE(num2, -, ,) , NX_PP_LESS_NUMBER(num1, y, n))
#define NX_PP_MORE(num1, num2, y, n)        NX_PP_HELPER(NX_PP_CLONE(num2, -, ,) , NX_PP_MORE_NUMBER(num1, y, n))
#define NX_PP_EQUA(num1, num2, y, n)        NX_PP_MORE(num1, num2, n, NX_PP_LESS(num1, num2, n, y))

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_RECUR(9, f, nul, c)
    -->
    f(c, f(c, f(c, f(c, f(c, f(c, f(c, f(c, f(c, nul)))))))))
*/

#define NX_PP_RECUR_1(data, ...)            NX_PP_A1(data)(NX_PP_B2(data), __VA_ARGS__)
#define NX_PP_RECUR_2(data)                 NX_PP_A1(data)(NX_PP_B2(data), NX_PP_A2(data))
#define NX_PP_RECUR(n, f, nul, ...)         NX_PP_NEST(n, NX_PP_RECUR_1, NX_PP_RECUR_2, NX_PP_VA, f, nul, __VA_ARGS__)

/*
    NX_PP_ORDER(f, 1, 2, 3, 4, 5, 6, 7, 8, 9)
    -->
    f(1, f(2, f(3, f(4, f(5, f(6, f(7, f(8, 9))))))))
*/

#define NX_PP_ORDER_1(data, ...)            NX_PP_A1(data)(NX_PP_A2(data), __VA_ARGS__)
#define NX_PP_ORDER_2(data)                 NX_PP_A1(data)(NX_PP_B1(data))
#define NX_PP_ORDER_3(...)                  NX_PP_A1(__VA_ARGS__), NX_PP_B2(__VA_ARGS__)
#define NX_PP_ORDER_N(n, f, ...)            NX_PP_NEST(n, NX_PP_ORDER_1, NX_PP_ORDER_2, NX_PP_ORDER_3, f, __VA_ARGS__)
#define NX_PP_ORDER(f, ...)                 NX_PP_ORDER_N(NX_PP_COUNT(NX_PP_B1(__VA_ARGS__)), f, __VA_ARGS__)

/*
    NX_PP_CALL(func, 1, 2, 3, 4, 5, 6, 7)
    -->
    func7(1, 2, 3, 4, 5, 6, 7)

    NX_PP_CALL(func)
    -->
    func0()
*/

#define NX_PP_CALL_1(f, ...)                NX_PP_VA(NX_PP_JOIN(f, NX_PP_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define NX_PP_CALL_2(f)                     NX_PP_VA(NX_PP_JOIN(f, 0)())
#define NX_PP_CALL_(n)                      NX_PP_JOIN(NX_PP_CALL_, NX_PP_LESS(1, n, 1, 2))
#define NX_PP_CALL_N(n, ...)                NX_PP_PROXY(NX_PP_CALL_(n), __VA_ARGS__)
#define NX_PP_CALL(...)                     NX_PP_CALL_N(NX_PP_COUNT(__VA_ARGS__), __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/*
    NX_PP_PARAM(par, int, char, long, double)
    -->
    int par1 , char par2 , long par3 , double par4
*/

#define NX_PP_PARAM_1(n, ...)               NX_PP_A1(NX_PP_B(n, __VA_ARGS__)) NX_PP_JOIN(NX_PP_A1(__VA_ARGS__), n)
#define NX_PP_PARAM_2(n, ...)               , NX_PP_PARAM_1(n, __VA_ARGS__)
#define NX_PP_PARAM(...) \
    NX_PP_REPEATEX(NX_PP_COUNT(NX_PP_B1(__VA_ARGS__)), NX_PP_PARAM_1, NX_PP_PARAM_2, __VA_ARGS__)

/*
    NX_PP_TYPE_1(3, T)
    -->
    T1 , T2 , T3

    NX_PP_TYPE_2(3, P, * par, = NULL)
    -->
    P1 * par1 = NULL, P2 * par2 = NULL, P3 * par3 = NULL
*/

#define NX_PP_TYPE_1_1(n, t1, ...)          NX_PP_VA(NX_PP_JOIN(t1, n) __VA_ARGS__)
#define NX_PP_TYPE_2_1(n, t1, t2, ...)      NX_PP_VA(NX_PP_JOIN(t1, n) NX_PP_JOIN(t2, n) __VA_ARGS__)
#define NX_PP_TYPE_3_1(n, t1, t2, t3, ...)  NX_PP_VA(NX_PP_JOIN(t1, n) NX_PP_JOIN(t2, n) NX_PP_JOIN(t3, n) __VA_ARGS__)

#define NX_PP_TYPE_1_2(n, t1, ...)          NX_PP_VA(, NX_PP_JOIN(t1, n) __VA_ARGS__)
#define NX_PP_TYPE_2_2(n, t1, t2, ...)      NX_PP_VA(, NX_PP_JOIN(t1, n) NX_PP_JOIN(t2, n) __VA_ARGS__)
#define NX_PP_TYPE_3_2(n, t1, t2, t3, ...)  NX_PP_VA(, NX_PP_JOIN(t1, n) NX_PP_JOIN(t2, n) NX_PP_JOIN(t3, n) __VA_ARGS__)

#define NX_PP_TYPE_1(n, t1, ...)            NX_PP_REPEATEX(n, NX_PP_TYPE_1_1, NX_PP_TYPE_1_2, t1, __VA_ARGS__)
#define NX_PP_TYPE_2(n, t1, t2, ...)        NX_PP_REPEATEX(n, NX_PP_TYPE_2_1, NX_PP_TYPE_2_2, t1, t2, __VA_ARGS__)
#define NX_PP_TYPE_3(n, t1, t2, t3, ...)    NX_PP_REPEATEX(n, NX_PP_TYPE_3_1, NX_PP_TYPE_3_2, t1, t2, t3, __VA_ARGS__)

#define NX_PP_TYPE_MAX_1(t1, ...)           NX_PP_TYPE_1(NX_PP_MAX, t1, __VA_ARGS__)
#define NX_PP_TYPE_MAX_2(t1, t2, ...)       NX_PP_TYPE_2(NX_PP_MAX, t1, t2, __VA_ARGS__)
#define NX_PP_TYPE_MAX_3(t1, t2, t3, ...)   NX_PP_TYPE_3(NX_PP_MAX, t1, t2, t3, __VA_ARGS__)
