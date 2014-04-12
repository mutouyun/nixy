#pragma once

#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////

namespace test_addressof
{
    class nonaddressable 
    {
    private:
        int operator&() const;
    };
}

void testAddressof(void)
{
    TEST_CASE();

    using namespace test_addressof;

    nonaddressable na;
    nonaddressable * naptr = nx::addressof(na);
    strout << naptr << endl;

    naptr = nx::addressof(naptr);

    int a[10] = {0};
    nx_auto(NX_UNUSED aptr, nx::addressof(a));
}

//////////////////////////////////////////////////////////////////////////

void testOperator(void)
{
    TEST_CASE();

    strout << NX_PP_STR(nx_operator(T, nx::unequal, nx::comparable, nx::operable)) << endl;
    strout << NX_PP_STR(nx_operatorex(T, double, nx::unequal, nx::comparable, nx::operable)) << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_initialize
{
    class Base
    {
    public:
        virtual void foo(void)
        {
            strout << "Base::foo" << endl;
        }

        virtual ~Base() {}

        void initialize(void)
        {
            foo();
        }
    };

    class Derived : public Base
    {
    public:
        void foo(void)
        {
            strout << "Derived::foo" << endl;
        }

        using Base::initialize;
    };

    struct POD
    {
        int   a_;
        long  b_;
        float c_;
        short d_;
    };

    class NoPOD
    {
    public:
        int   a_;
        long  b_;
        float c_;
        short d_;

        NoPOD()
            : a_(0), b_(9), c_(0), d_(0)
        {}
    };
}

void testInitialize(void)
{
    TEST_CASE();

    using namespace test_initialize;

    Base* baseptr = nx::initialize(new Derived);
    delete baseptr;

    int i = 123;
    nx::initialize(i);
    strout << i << endl;

    int arr[10] = {1, 2, 3, 4, 5, 6, 7};
    nx::initialize(arr);
    nx_foreach(i, arr)
    {
        strout << i << " ";
    }
    strout << endl;

    POD pod = {1, 2, 3, 4};
    nx::initialize(pod);
    strout << pod.a_ << " ";
    strout << pod.b_ << " ";
    strout << pod.c_ << " ";
    strout << pod.d_ << " ";
    strout << endl;

    NoPOD nopod;
    nopod.b_ = nopod.a_ = nopod.d_ = 123;
    nopod.c_ = 123;
    nx::initialize(nopod);
    strout << nopod.a_ << " ";
    strout << nopod.b_ << " ";
    strout << nopod.c_ << " ";
    strout << nopod.d_ << " ";
    strout << endl;
}

//////////////////////////////////////////////////////////////////////////

void testCountOf(void)
{
    TEST_CASE();

    int NX_UNUSED a[10];
    int NX_UNUSED b[nx_countof(a)];
    strout << nx_countof(b) << endl;
}

//////////////////////////////////////////////////////////////////////////

void testLimitOf(void)
{
    TEST_CASE();

    strout << "The lower limit of nx::ullong ->:\t" << nx::limit_of<nx::ullong>::lower << endl;
    strout << "The upper limit of nx::ullong ->:\t" << nx::limit_of<nx::ullong>::upper << endl;
    strout << "The lower limit of nx::llong ->:\t"  << nx::limit_of<nx::llong>::lower << endl;
    strout << "The upper limit of nx::llong ->:\t"  << nx::limit_of<nx::llong>::upper << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_safe_bool
{
    class TestBool : public nx::safe_bool<TestBool>
    {
    public:
        bool checkSafeBool(void) const
        {
            return false;
        }
    };
}

void testSafeBool(void)
{
    TEST_CASE();

    using namespace test_safe_bool;

    TestBool tb;
    if (tb)
        strout << "Y: TestBool" << endl;
    else
        strout << "N: TestBool" << endl;

    if (false != tb)
        strout << "T: TestBool" << endl;
    else
        strout << "F: TestBool" << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_refer
{
    class A
    {
    public:
        int a_;

    public:
        A(void) : a_(0)
        { strout << "cont A" << endl; }
        A(int a) : a_(a)
        { strout << "cont A " << a_ << endl; }
        A(const A& a)
        { strout << "copy A " << (a_ = a.a_) << endl; }
        ~A(void)
        { strout << "dest A " << a_ << endl; a_ = 0; }
    };

    class B
    {
    public:
        int a_;

    public:
        B(void) : a_(0)
        { strout << "cont B" << endl; }
        B(int a) : a_(a)
        { strout << "cont B " << a_ << endl; }
        B(const A& a)
        { strout << "copy B " << (a_ = a.a_) << endl; }
        ~B(void)
        { strout << "dest B " << a_ << endl; a_ = 0; }
    };

    void processValue(int& /*val*/)
    {
        strout << "processValue() ->: int&" << endl;
    }

    void processValue(const int& /*val*/)
    {
        strout << "processValue() ->: const int&" << endl;
    }

    template <typename T>
    void forwardValue(T val)
    {
        processValue(val);
    }
}

void testRefer(void)
{
    TEST_CASE();

    using namespace test_refer;
    {
        A a; B b(1);
        nx_auto(NX_UNUSED ra, nx::ref(a));
        nx_auto(NX_UNUSED rb, nx::ref(b));
    }
    strout << endl;
    {
        int a = 0;
        const int &b = 1;

        forwardValue(nx::ref(a));
        forwardValue(nx::ref(b));
        forwardValue(nx::ref(2));
    }
}

//////////////////////////////////////////////////////////////////////////

namespace test_rvalue
{
    struct Big
    {
        char* str_;

        void clear(void)
        {
            if (!str_) return;
            str_[0] = '\0';
        }

        Big(void)
            : str_(new char[1024])
        {
            str_[0] = '\0';
            strout << "Constructor" << endl;
        }

        Big(const char* str)
            : str_(new char[1024])
        {
            if (str) strcpy(str_, str);
            strout << "Constructor " << str_ << endl;
        }

        Big(const Big& big)
            : str_(new char[1024])
        {
            if (big.str_)
                memcpy(str_, big.str_, 1024);
            else
                clear();
            strout << "Copy Constructor " << str_ << endl;
        }

        Big(nx_rref(Big) big)
            : str_(nx::nulptr)
        {
            swap(nx::moved(big));
            strout << "Move Constructor ";
            if (str_) strout << str_;
            strout << endl;
        }

        const Big& operator=(const char* str)
        {
            if (str) strcpy(str_, str);
            return (*this);
        }

        const Big& operator=(const Big& big)
        {
            if (big.str_)
                memcpy(str_, big.str_, 1024);
            else
                clear();
            strout << "Copy Assignment " << str_ << endl;
            return (*this);
        }

        const Big& operator=(nx_rref(Big) big)
        {
            swap(nx::moved(big));
            strout << "Move Assignment ";
            if (str_) strout << str_;
            strout << endl;
            return (*this);
        }

        ~Big()
        {
            if (!str_) return;
            strout << "Destructor " << str_ << endl;
            delete [] str_;
        }

        void swap(Big& rhs)
        {
            nx::swap(str_, rhs.str_);
        }
    };

    Big testCopy(void)
    {
        Big tmp("Hello Copy!");
        if (true) return tmp;
        else      return Big();
    }

    nx_rval(Big) testMove(void)
    {
        Big tmp("Hello Move!");
        if (true) return nx::move(tmp);
        else      return Big();
    }
}

#include "nixycore/container/container.h"

void testRvalue(void)
{
    TEST_CASE();

    using namespace test_rvalue;
    Big big("Default data");
    strout << endl;
    {
        big = testCopy();
    }
    big = "Default data";
    strout << endl;
    {
        big = testMove();
    }
    big = "Default data";
    strout << endl;
    {
        Big tmp("Temp data");
        big = tmp;
    }
    big = "Default data";
    strout << endl;
    {
        Big tmp("Temp data");
        big = nx::move(tmp);
    }
    big = "Default data";
    strout << endl;

    int a = 0;
    nx_rval(int) b(a), c(b);
    a = nx::moved(c);
    int NX_UNUSED d = nx::move(a);

    nx::list<Big> q1, q2;
    q2.push_back("It's BIG");
    q1 = nx::move(q2);
    strout << "q1: " << q1.back().str_ << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_valid
{
    class A
    {
    public:
        int a_;

    public:
        A(void) : a_(0)
        { strout << "cont A" << endl; }
        A(int a) : a_(a)
        { strout << "cont A " << a_ << endl; }
        A(const A& a)
        { strout << "copy A " << (a_ = a.a_) << endl; }
        ~A(void)
        { strout << "dest A " << a_ << endl; a_ = 0; }
    };
}

void testValid(void)
{
    TEST_CASE();

    using namespace test_valid;

    A a(1);

    nx::valid<A> vb = nx::none;
    nx::valid<const A&> va = vb/*nx::none*/;
    nx::valid<A&> vc = nx::none;

    va = a;
    if (va || vb)
        strout << "yes!" << endl;

    vb = a; vc = a;
    (*vb).a_ = 2;
    *vc = *vb;
    va = vb;

    strout << "reset vb -> ";
    vb.reset();
    strout << "reset va -> ";
    va.reset();
    strout << "reset fini" << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_tuple
{
    nx::tuple<int, char, double> func(void)
    {
        int i = 1; char c = 2; double d = 3;
        return nx::tie(i, c, d);
    }
}

void testTuple(void)
{
    TEST_CASE();

    using namespace test_tuple;

    int i = 0; char c = 0; double d = 0;
    nx::tie(i, c, d) = func();
    nx::tuple<int, char, double> td(5, 5, 5);
    nx_auto(tr, nx::tie(i, c, d));
    nx::swap(td, tr);
    strout << tr.at<0>() << " " << tr.at<1>() << " " << tr.at<2>() << " " << endl;
    strout << *td << " " << td.at<char>() << " " << td.at<double>() << " " << endl;

    nx::tuple<int, char, char> td2(4, '5', '6');
    strout << *td2 << " " << td2.at<char>() /*<< " " << td2.at<double>()*/ << " " << endl;

    nx::tuple<> xx;
    strout << xx.length() << endl;
}

//////////////////////////////////////////////////////////////////////////

void testUtility(void)
{
    TEST_FUNCTION();

    //testAddressof();
    //testOperator();
    //testInitialize();
    //testCountOf();
    //testLimitOf();
    //testSafeBool();
    //testRefer();
    testRvalue();
    //testValid();
    testTuple();
}
