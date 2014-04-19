#pragma once

#include "nixycore/finalizer/finalizer.h"

//////////////////////////////////////////////////////////////////////////

namespace test_scopeguard
{
    void onScopeExit(void)
    {
        strout << NX__FUNCTION__ << endl;
    }

    void onSoleExit(void)
    {
        strout << NX__FUNCTION__ << endl;
    }
}

void testScopeGuard(void)
{
    TEST_CASE();

    using namespace test_scopeguard;

    {
        nx_guard_scope(onScopeExit);
        static nx_guard_scope(onSoleExit);

#   ifdef NX_SP_CXX11_LAMBDA
        nx_guard_scope([]()
        {
            strout << NX__FUNCTION__ << endl;
        });
#   endif
    }

    strout << __FUNCTION__ << " ending..." << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_gc
{
    template <typename T>
    void new_dest(T* p)
    {
        if (!p) return;
        strout << NX__FUNCTION__ << " ->: ";
        delete p;
    }

    class A
    {
    public:
        int a_;

    public:
        A(int a = 0) : a_(a)
        { strout << "cont A " << a_ << endl; }
        ~A(void)
        { strout << "dest A " << a_ << endl; }
    };

    class B
    {
    public:
        A* p_;

    public:
        B(int a = 0) : p_(nx::nulptr)
        {
            nx_gc_scope() = this;
            gc(p_)(new A(a), new_dest<A>);
            strout << "cont B " << p_->a_ << endl;
        }
        ~B(void)
        {
            strout << "dest B " << p_->a_ << endl;
        }
    };

    void func_scope(B*(& pd))
    {
        nx_gc_scope();
        gc(pd)(new B(4), new_dest<B>);
    }
}

void testGC(void)
{
    TEST_CASE();

    using namespace test_gc;
    {
        nx_gc_scope();

        A* pa = nx::nulptr;
        A* pb = nx::nulptr;

        gc(pa)(new A(1), new_dest<A>);
        gc(pb) = nx::alloc<A>(2);
        gc(pb) = pa;

        B* pd = nx::nulptr;
        gc(pd) = nx::alloc<B>(3);

        strout << "...scope in..." << endl;
        func_scope(pd);
        strout << "...scope out..." << endl;

        gc(pd) = nx::alloc<B>(5);

        strout << "...start dest..." << endl;
    }
    strout << endl << "Finish Scope" << endl << endl;
    {
        nx_gc_scope();
        B* NX_UNUSED pb = gc(nx::alloc<B>(2));
        A* NX_UNUSED pa = gc(new A(1), new_dest<A>);
    }
}

//////////////////////////////////////////////////////////////////////////

void testFinalizer(void)
{
    TEST_FUNCTION();

    testScopeGuard();
    //testGC();
}
