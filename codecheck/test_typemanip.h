#pragma once

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/countof.h"

#include <typeinfo>

//////////////////////////////////////////////////////////////////////////

namespace test_typemanip
{
    class M;

    enum E
    {
        E1, E2
    };

    union U
    {
        int i;
        long l;
        char c;
        void* p;
    };
}

//////////////////////////////////////////////////////////////////////////

void testTypeQualifier(void)
{
    TEST_CASE();

    using namespace test_typemanip;

    strout << nx::is_const<int()>::value << " " << nx::is_const<int(&)()>::value << endl;
    strout << nx::is_const<int>::value << " " << nx::is_const<const int>::value << endl;
    strout << nx::is_const<int&>::value << " " << nx::is_const<const int&>::value << endl;
    strout << nx::is_const<int[]>::value << " " << nx::is_const<const int[]>::value << endl;
    strout << nx::is_const<int[5]>::value << " " << nx::is_const<const int[5]>::value << endl;
    strout << nx::is_const<int*>::value << " " << nx::is_const<const int*>::value << " " << nx::is_const<int* const>::value << endl;
    strout << nx::is_const<int M::*>::value << " " << nx::is_const<const int M::*>::value << " " << nx::is_const<int M::* const>::value << endl;
    strout << nx::is_const<int(*)()>::value << " " << nx::is_const<int(* const)()>::value << endl;
    strout << nx::is_const<int(M::*)()>::value << " " << nx::is_const<int(M::* const)()>::value << endl;
    strout << nx::is_const<int(M::*)() const>::value << " " << nx::is_const<int(M::* const)() const>::value << endl;
    strout << nx::is_const<int(M::*)(int) volatile>::value << " " << nx::is_const<int(M::* const)(int) volatile>::value << endl;
    strout << nx::is_const<int(M::*)(int) const volatile>::value << " " << nx::is_const<int(M::* const volatile)(int) const volatile>::value << endl;

    strout << endl;

    strout << typeid(nx::rm_const<int>::type_t).name() << endl;
    strout << typeid(nx::rm_const<const int>::type_t).name() << endl;
    strout << typeid(nx::rm_const<const int&>::type_t).name() << endl;
    strout << typeid(nx::rm_const<const int[]>::type_t).name() << endl;
    strout << typeid(nx::rm_const<const int[5]>::type_t).name() << endl;
    strout << typeid(nx::rm_const<const int*>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int* const>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int M::* const>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int(* const)()>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int(M::* const)()>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int(M::* const)() const>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int(M::* const)() volatile>::type_t).name() << endl;
    strout << typeid(nx::rm_const<int(M::* const volatile)(int) const volatile>::type_t).name() << endl;
}

//////////////////////////////////////////////////////////////////////////

void testTypeBehavior(void)
{
    TEST_CASE();

    using namespace test_typemanip;

    strout << nx::is_reference<int>::value << endl;
    strout << nx::is_reference<int&>::value << endl;
    strout << nx::is_reference<const int*(&)>::value << endl;
    strout << nx::is_reference<int(&)[5]>::value << endl;
    strout << nx::is_reference<int(&)(int)>::value << endl;

    strout << endl;

    strout << nx::is_array<int>::value << " " << nx::is_array<int*>::value << endl;
    strout << nx::is_array<const int*[]>::value << endl;
    strout << nx::is_array<volatile int[5]>::value << endl;

    strout << endl;

    strout << nx::is_pointer<int>::value << endl;
    strout << nx::is_pointer<int*>::value << " " << nx::is_pointer<const int*>::value << " " << nx::is_pointer<int* const>::value << endl;
    strout << nx::is_pointer<int M::*>::value << " " << nx::is_pointer<int M::* const>::value << endl;
    strout << nx::is_pointer<int(*)()>::value << " " << nx::is_pointer<int(* const)()>::value << endl;
    strout << nx::is_pointer<int(M::*)()>::value << " " << nx::is_pointer<int(M::* const)()>::value << endl;
    strout << nx::is_pointer<int(M::*)() const>::value << " " << nx::is_pointer<int(M::* const)() const>::value << endl;
    strout << nx::is_pointer<int(M::*)() volatile>::value << " " << nx::is_pointer<int(M::* volatile)() volatile>::value << endl;
    strout << nx::is_pointer<int(M::*)(int) const volatile>::value << " " << nx::is_pointer<int(M::* const volatile)(int) const volatile>::value << endl;
    strout << typeid(nx::rm_pointer<int(M::*)(int) const volatile>::type_t).name() << endl;

    strout << nx::is_union<M>::value << endl;
}

//////////////////////////////////////////////////////////////////////////

void testTypeManip(void)
{
    TEST_FUNCTION();

    testTypeQualifier();
    testTypeBehavior();

    TEST_CASE();

    using namespace test_typemanip;

    typedef nx::types<char, void*, long, char*, void**, size_t*, int, long, long long>::type_t typelist_t;
    typedef nx::types_compact<typelist_t>::type_t tl_t;

    nx::types_at<tl_t, 2>::type_t a[nx::types_len<tl_t>::value];
    a[0] = (nx::types_at<tl_t, 2>::type_t)nx::types_find<tl_t, long>::value;
    strout << nx_countof(a) << " " << a[0] << endl;

    nx_auto(pcout, &(strout));
    strout << "point size is "  << sizeof(pcout) << endl;
}
