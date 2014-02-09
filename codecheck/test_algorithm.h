#pragma once

#include "nixycore/algorithm/algorithm.h"
#include "nixycore/container/container.h"
#include "nixycore/pattern/iterator.h"

//////////////////////////////////////////////////////////////////////////

namespace test_assign
{
    double getNumContainer(void)
    {
        strout << __FUNCTION__ << endl;
        return 10.0;
    }

    list<double>/*&*/ getLstContainer(void)
    {
        strout << __FUNCTION__ << endl;

        static struct List
        {
            list<double> list_;
            List()
            { nx_assign(list_, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9); }
        } lst;

        return lst.list_;
    }

    struct Foo
    {
        int i_;
        Foo(int i = 0) : i_(i) {}
    };
}

void testAssign(void)
{
    TEST_CASE();

    using namespace test_assign;

    int a = 0, b = 0, c = 0, d = 0;

    nx::vector<int> ivct;
    nx_assign(ivct, 3, 2, 1);
    nx_assign(ivct) >> a >> b >> c >> d;
    nx_foreach(i, ivct)
    {
        strout << i << " ";
    }
    strout << endl;
    strout << a << " " << b << " " << c << " " << d << endl;

    int numarr[8] = {1, 2, 3, 4, 5, 6};
    nx_assign(numarr, 3, 2, 1);
    nx_assign(numarr) >> a >> b >> c >> d;
    nx_foreach(i, numarr)
    {
        strout << i << " ";
    }
    strout << endl;
    strout << a << " " << b << " " << c << " " << d << endl;

    nx_auto(n, 2);
    nx_assign(n, 3, 2, 1);
    nx_assign(n) >> a >> b >> c >> d;
    nx_foreach(i, n)
    {
        strout << i << " ";
    }
    strout << endl;
    strout << a << " " << b << " " << c << " " << d << endl;

    strout << endl;
    nx_foreach(i, getNumContainer())
    {
        strout << i << " ";
    }
    strout << endl;
    nx_foreach(i, getLstContainer())
    {
        strout << i << " ";
    }
    strout << endl << endl;

    strout << "foo_list" << endl;
    nx::list<Foo> foo_list;
    nx_assign(foo_list, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    nx_foreach(&i, foo_list)
    {
        strout << i.i_ << " ";
        i.i_ = 9;
    }
    strout << endl;
    nx_foreach(const &i, foo_list)
    {
        strout << i.i_ << " ";
    }
    strout << endl;

    strout << "foo_map" << endl;
    nx::map<int, int> foo_map;
    nx_assign(foo_map)(1, 2)(2, 3)(3, 4)(4, 5)(5, 6);
    nx_foreach(const &i, foo_map)
    {
        strout << "(" << i.first << ", " << i.second << ") ";
    }
    strout << endl;
}

//////////////////////////////////////////////////////////////////////////

void testSeries(void)
{
    TEST_CASE();

    int n = 20;
    strout << "Fibonacci(0 - " << n << ") ->:" << endl;
    nx::iterator<nx::by_iter_fibonacci<nx::llong> > iter;
    nx_foreach(i, n, ++iter)
    {
        strout << i << ":\t" << *iter << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

namespace test_algorithm
{
    class A
    {
    public:
        int a_;

    public:
        A(int i) : a_(i) {}

        void swap(A& r)
        {
            nx::swap(a_, r.a_);
        }
    };

    class B
    {
    public:
        int a_;

    public:
        B(int i) : a_(i) {}

        B& swap(B& r)
        {
            nx::swap(a_, r.a_);
            return (*this);
        }
    };
}

void testAlgorithm(void)
{
    TEST_FUNCTION();

    testAssign();
    testSeries();

    TEST_CASE();

    using namespace test_algorithm;
    {
        int x = 1, y = 2;
        nx::swap(x, y);
        strout << x << " " << y << endl;
    }
    {
        A x(1), y(2);
        nx::swap(x, y);
        strout << x.a_ << " " << y.a_ << endl;
    }
    {
        B x(1), y(2);
        nx::swap(x, y);
        strout << x.a_ << " " << y.a_ << endl;
    }
}
