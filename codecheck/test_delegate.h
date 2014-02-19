#pragma once

#include "nixycore/delegate/delegate.h"

//////////////////////////////////////////////////////////////////////////

namespace test_delegate
{
    void func0_0(void)
    {
        strout << NX__FUNCTION__ << endl;
    }

    int func0_1(void)
    {
        strout << NX__FUNCTION__ << " ->: ";
        return 0;
    }

    int func1(long a)
    {
        strout << NX__FUNCTION__ << "(" << a << ")" << " ->: ";
        return 1;
    }

    static int func2(long a, int b)
    {
        strout << NX__FUNCTION__ << "(" << a << ", " << b << ")" << " ->: ";
        return 2;
    }

    class A
    {
    public:
        virtual int func(long a, int b, long c, int d)
        {
            strout << NX__FUNCTION__ << "(" << a << ", " 
                                          << b << ", " 
                                          << c << ", " 
                                          << d << ")" << " ->: ";
            return 4;
        }
    };

    class B : public A
    {
    public:
        int func(long a, int b, long c, int d)
        {
            strout << NX__FUNCTION__ << "(" << a << ", " 
                << b << ", " 
                << c << ", " 
                << d << ")" << " ->: ";
            return 4;
        }

        static int funcS(long a)
        {
            strout << NX__FUNCTION__ << "(" << a << ")" << " ->: ";
            return 1;
        }
    };

    class Func
    {
    public:
        int operator()(void)
        {
            strout << NX__FUNCTION__ << " ->: ";
            return 0;
        }
    };

    class Func1
    {
    public:
        const char* operator()(string& s)
        {
            strout << NX__FUNCTION__ << "(" << s.c_str() << ")" << " ->: ";
            return s.c_str();
        }
    };

    class Func2
    {
    public:
        Func2(void) {}
        int operator()(void) const volatile
        {
            strout << NX__FUNCTION__ << " ->: ";
            return 0;
        }
    };
}

//////////////////////////////////////////////////////////////////////////

void testFunctor(void)
{
    TEST_CASE();

    using namespace test_delegate;
    {
        nx::functor<void(void)> fr(&func0_0);
        fr();
        fr = &func0_1;
        fr();
        strout << endl;

        nx::functor<void(void)> fr2;
        fr2 = nx::move(fr);
    }
    {
        nx::functor<int(void)> fr(NULL);
        fr = &func0_1;
        strout << fr() << endl;
        fr = NULL;
        fr = nx::nulptr;
        fr = nx::none;
        if (!fr)
            strout << "empty functor" << endl;
    }
    {
        nx::functor<int(int)> fr(&func1);
        strout << fr(1) << endl;
    }
    {
        nx::functor<int(long, int)> fr(&func2);
        strout << fr(1, 2) << endl;
    }
    {
        Func f;
        nx::functor<int(void)> fr(&f);
        strout << fr() << endl;
    }
    strout << endl;
    {
        Func f;
        nx::functor<int(void)> fr1(&f);
        nx::functor<int(void)> fr2(&func0_1);
        nx::swap(fr1, fr2);
        strout << fr1() << endl;
        if (fr1 != fr2) fr1 = fr2;
        if (fr1 == fr2)
        {
            strout << "fr1 == fr2" << endl;
            strout << fr1() << endl;
        }
    }
    strout << endl;
    {
        nx::functor<int(long, int)> fr;
        nx_auto(const fx, &func2);
        strout << (fr = fx)(1, 2) << endl;
    }
    {
        Func f;
        nx::functor<int(void)> fr;
        nx_auto(const fx, &f);
        strout << (fr = fx)() << endl;
        const Func2 f2;
        fr = f2;
        strout << fr() << endl;
    }
    {
        Func1 f;
        nx::functor<const char*(string&)> fr;
        string s("Hello functor");
        strout << (fr = f)(s) << endl;
    }
    strout << endl;
    {
        B b;
        A* pa = &b;
        nx::functor<int(long, int, int, int)> fr;
        strout << fr.bind(&A::func, pa)(4, 3, 2, 1) << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void testBind(void)
{
    TEST_CASE();

    using namespace test_delegate;

    Func f;
    Func1 f1;
    Func2 f2;
    B a;

    {
        strout << nx::bind(&func1)(1) << endl;
        strout << nx::bind<int>(&f)() << endl;
        strout << nx::bind(&A::func, &a)(4, 3, 2, 1) << endl;
    }
    strout << endl;
    {
        strout << nx::bind(&func1, 2)() << endl;
        strout << nx::bind(&func2, nx::_2, nx::_2)(1, 2) << endl;
        strout << nx::bind(&A::func, &a, nx::_4, nx::_3, nx::_2, nx::_1)(4, 3, 2, 1) << endl;
        string s("Hello bind");
        strout << nx::bind<const char*>(&f1, nx::ref(s))() << endl;
    }
    strout << endl;
    {
        nx::functor<int()> fr = nx::bind(&B::funcS, 4);
        strout << fr() << endl;
    }
    strout << endl;
    {
        nx::functor<int(void)> fr = nx::bind<int>(f2);
        strout << fr() << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

namespace test_signal
{
    class Foo
    {
    public:
        nx::signal<void(int, int, int, int)> notify;
    };
}

void testSignal(void)
{
    TEST_CASE();

    using namespace test_delegate;
    using namespace test_signal;

    Foo foo;

    strout << endl << "1st notify ..." << endl;
    foo.notify(3, 1, 2, 4);

    strout << endl << "2nd notify ..." << endl;
    B x1; A x2;
    foo.notify.connect(&x1, &A::func);
    foo.notify.connect(&x2, &A::func);
    foo.notify(3, 1, 2, 4);

    strout << endl << "3rd notify ..." << endl;
    foo.notify.clear();
    foo.notify(3, 1, 2, 4);
}

//////////////////////////////////////////////////////////////////////////

void testDelegate(void)
{
    TEST_FUNCTION();

    //testFunctor();
    //testBind();
    testSignal();
}
