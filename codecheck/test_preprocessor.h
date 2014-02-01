#pragma once

#include "general/general.h"
#include "preprocessor/preprocessor.h"
#include "preprocessor/pprepeat.h"

//////////////////////////////////////////////////////////////////////////

void testPreprocessor(void)
{
    TEST_FUNCTION();

    strout << NX_PP_STR(NX_PP_REPEAT(5, f, d1, d2)) << endl;
    strout << NX_PP_STR(NX_PP_NEST(3, func, last, nest, xx)) << endl;

    strout << NX_PP_STR(NX_PP_CLONE(6, c, ,)) << endl;

    strout << NX_PP_STR(NX_PP_REV(1, 2, 3, 4, 5, 6, 7, 8, 9)) << endl;
    strout << NX_PP_STR(NX_PP_LIMIT(3, a, b, c, d, e, f)) << endl;

    strout << NX_PP_STR(NX_PP_INC(5)) << endl;
    strout << NX_PP_STR(NX_PP_DEC(5)) << endl;
    strout << NX_PP_STR(NX_PP_ADD(3, 4)) << endl;
    strout << NX_PP_STR(NX_PP_SUB(5, 3)) << endl;

    strout << NX_PP_STR(NX_PP_LESS(3, 4, Yes, No)) << endl;
    strout << NX_PP_STR(NX_PP_MORE(3, 4, Yes, No)) << endl;
    strout << NX_PP_STR(NX_PP_EQUA(4, 4, Yes, No)) << endl;

    strout << NX_PP_STR(NX_PP_RECUR(9, f, nul, c)) << endl;
    strout << NX_PP_STR(NX_PP_ORDER(f, 1, 2, 3, 4, 5, 6, 7, 8, 9)) << endl;
    strout << NX_PP_STR(NX_PP_CALL(func)) << endl;

    strout << NX_PP_STR(NX_PP_PARAM(par, int, char, long, double)) << endl;

    strout << NX_PP_STR(NX_PP_TYPE_1(3, T)) << endl;
    strout << NX_PP_STR(NX_PP_TYPE_2(3, P, * par, = NULL)) << endl;
    strout << NX_PP_STR(NX_PP_TYPE_1(1, typename P)) << endl;

//#define NX_REPEAT_2(f1, f2, ...)    f1(1, __VA_ARGS__) f2(2, __VA_ARGS__)
//#define NX_MULT_2(f)                f(1) f(2)
//
//#define NX_TYPE_1_1(n, t1, ...)     NX_PP_VA(NX_PP_JOIN(t1, n) __VA_ARGS__)
//#define NX_TYPE_1_2(n, t1, ...)     NX_PP_VA(, NX_PP_JOIN(t1, n) __VA_ARGS__)
//
//#define NX_RP(n)                    NX_REPEAT_2(NX_TYPE_1_1, NX_TYPE_1_2, T)
//
//    NX_MULT_2(NX_RP)
}
