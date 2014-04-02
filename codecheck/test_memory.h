#pragma once

#include "nixycore/memory/memory.h"
#include "nixycore/time/stopwatch.h"
#include "nixycore/random/random.h"
#include "nixycore/thread/thread.h"

#if (NX_CC_MSVC == 1400) || defined(NX_OS_WINCE)
#define NO_TEST_NEDMALLOC
#endif

#ifndef NO_TEST_NEDMALLOC
#ifdef NX_CC_GNUC
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#   ifdef NX_CC_CLANG
#   pragma GCC diagnostic ignored "-Wself-assign"
#   endif
#endif
#include "nedmalloc/nedmalloc.c"
#endif

//////////////////////////////////////////////////////////////////////////

namespace test_mempool
{
#ifdef NX_OS_WINCE
    const int TestCont = 100;
    const int TestLast = 100;
#elif NDEBUG
    const int TestCont = 10000;
    const int TestLast = 1000;
#else
    const int TestCont = 1000;
    const int TestLast = 1000;
#endif
    const int TestSMin = /*1*/4;
    const int TestSMax = /*4*//*16*//*32*/256/*1024*/;

    nx::size_t size[TestLast] = {0};
    nx::size_t indx[TestLast] = {0};
    nx::stopwatch<> sw;

    struct test_t
    {
        void*  p_;
        size_t s_;
        test_t(void) : p_(0), s_(0) {}
    };

    void init(int n = 1)
    {
        char str[256];
        sprintf(str, "Cycles:\t\t%d\nContinuous:\t%d\nAlloc Size:\t%dByte-%dByte\nThreads:\t%d\n",
            TestCont, TestLast, TestSMin * sizeof(nx::byte), TestSMax * sizeof(nx::byte), n);
        strout << str << endl;

        nx::random<> rdm(TestSMin, TestSMax);
        for(int i = 0; i < TestLast; ++i)
            rdm.roll(size[i]);

        rdm.range(0, TestLast);
        for(int i = 0; i < TestLast; ++i)
            rdm.roll(indx[i]);
    }

#define TEST_MEMPOOL(alloc_name, test_count) do { \
    for(int i = 0; i < (int)(test_count / 2); ++i) \
    { \
        alloc_name alc; \
        for(int x = 0; x < 2; ++x) \
        for(int n = 0; n < TestLast; ++n) \
        { \
            test_t& p = test[indx[n]]; \
            if (p.p_) \
            { \
                alc.free(p.p_, p.s_); \
                p.s_ = 0; \
                p.p_ = 0; \
            } \
            else \
            { \
                p.p_ = alc.alloc(size[n]); \
                p.s_ = size[n]; \
            } \
        } \
    } } while(0)

    template <typename A>
    void start(const char* out)
    {
        test_t test[TestLast];
        strout << out;

        sw.start();
        TEST_MEMPOOL(A, TestCont);
        strout << sw.value() * 1000 << " ms" << endl;
    }

    struct new_alloc
    {
        void* alloc(size_t size)
        {
            return new nx::byte[size];
        }
        void free(void* p, size_t /*size*/)
        {
            delete [] (nx::byte*)p;
        }
    };

    struct mempool_alloc
    {
        nx::mem_pool<> pool_;
        void* alloc(size_t size)
        {
            return pool_.alloc(size);
        }
        void free(void* p, size_t size)
        {
            pool_.free(p, size);
        }
    };

    struct unfixed_alloc
    {
        nx::unfixed_pool<> pool_;
        void* alloc(size_t size)
        {
            return pool_.alloc(size);
        }
        void free(void* /*p*/, size_t /*size*/)
        {
            // Do nothing
        }
    };

    struct mem_alloc
    {
        void* alloc(size_t size)
        {
            return nx::alloc(size);
        }
        void free(void* p, size_t size)
        {
            nx::free(p, size);
        }
    };

#ifndef NO_TEST_NEDMALLOC
    struct ned_alloc
    {
        void* alloc(size_t size)
        {
            return nedalloc::nedmalloc(size);
        }
        void free(void* p, size_t /*size*/)
        {
            nedalloc::nedfree(p);
        }
    };
#endif
}

void testMemPool(void)
{
    TEST_CASE();

    using namespace test_mempool;

    init();

    start<unfixed_alloc>("Start for nx::unfixed_pool...\t");
    start<mempool_alloc>("Start for nx::mem_pool...\t");
    start<mem_alloc>    ("Start for nx::alloc/free...\t");
#ifndef NO_TEST_NEDMALLOC
    start<ned_alloc>    ("Start for ned_alloc/free...\t");
#endif
    start<new_alloc>    ("Start for new && delete...\t");
}

//////////////////////////////////////////////////////////////////////////

namespace test_alloc
{
    nx::thread_ops::handle_t hd[10] = {0};

#define THREAD_ALLOC(alloc_name) \
    NX_THREAD_PROC(proc_##alloc_name, alcptr) \
    { \
        using namespace test_mempool; \
        alloc_name& alc = *(alloc_name*)alcptr; \
        test_t test[TestLast]; \
        TEST_MEMPOOL((void), TestCont / nx_countof(hd)); \
        return 0; \
    }
    THREAD_ALLOC(new_alloc)
    THREAD_ALLOC(mem_alloc)
#ifndef NO_TEST_NEDMALLOC
    THREAD_ALLOC(ned_alloc)
#endif
#undef THREAD_ALLOC

#define START_ALLOC(alloc_name) \
    void start_##alloc_name(const char* out) \
    { \
        using namespace test_mempool; \
        alloc_name alc; \
        strout << out; \
        sw.start(); \
        nx_foreach(i, nx_countof(hd)) hd[i] = nx::thread_ops::create(proc_##alloc_name, &alc); \
        nx_foreach(i, nx_countof(hd)) nx::thread_ops::join(hd[i]); \
        strout << sw.value() * 1000 << " ms" << endl; \
    }
    START_ALLOC(new_alloc)
    START_ALLOC(mem_alloc)
#ifndef NO_TEST_NEDMALLOC
    START_ALLOC(ned_alloc)
#endif
#undef START_ALLOC
}

void testAlloc(void)
{
    TEST_CASE();

    using namespace test_mempool;
    using namespace test_alloc;

    init(nx_countof(hd));

    start_mem_alloc  ("Start for nx::alloc/free...\t");
#ifndef NO_TEST_NEDMALLOC
    start_ned_alloc  ("Start for ned_alloc/free...\t");
#endif
    start_new_alloc  ("Start for new && delete...\t");
}

//////////////////////////////////////////////////////////////////////////

namespace test_memguard
{
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

    void new_dest(A* p)
    {
        strout << NX__FUNCTION__ << " ->: ";
        delete p;
    }
}

void testMemGuard(void)
{
    TEST_CASE();

    using namespace test_memguard;

    A* p1 = nx::alloc<A>(1);
    nx_guard_scope(p1);

    A* p2 = new A(2);
    nx_guard_scope(p2, new_dest);

    A* p3 = nx::alloc<A>(3);
    nx_guard_scope(p3);

    strout << "...Do something..." << endl;
}

//////////////////////////////////////////////////////////////////////////

void testPointer(void)
{
    TEST_CASE();

    using namespace test_memguard;
    {
        nx::pointer<void> p1(nx::alloc(10), 10);
        nx::pointer<void> p2(p1);
        p1 = p2;
        p2.set(nx::alloc(20), 20);
        nx::swap(p1, p2);
    }
    {
        nx::pointer<A> p1(new A(1), new_dest);

        nx::pointer<int[2]> p2 = nx::alloc<int[2]>();
        (*p2)[0] = 2;
        (*p2)[1] = 3;

        p1 = nx::pointer_cast<nx_typeof(p1)>(p2);
        strout << "pointer_cast: p1->a_ == " << p1->a_ << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void testObjectPool(void)
{
    TEST_CASE();

    using namespace test_memguard;
    {
        nx::object_pool<A> pool;
        A* p0 = pool.alloc();
        A* p1 = pool.alloc();
        strout << "p0->a_ == " << p0->a_ << endl;
        strout << "p1->a_ == " << p1->a_ << endl;
        pool.free(p0);
        pool.free(p1);
    }
    strout << "================" << endl;
    {
        nx::object_pool<A> pool(123);
        A* p0 = pool.alloc();
        A* p1 = pool.alloc();
        strout << "p0->a_ == " << p0->a_ << endl;
        strout << "p1->a_ == " << p1->a_ << endl;
        pool.free(p0);
        pool.free(p1);
    }
}

//////////////////////////////////////////////////////////////////////////

void testMemory(void)
{
    TEST_FUNCTION();

    testMemPool();
    testAlloc();
    //testMemGuard();
    //testPointer();
    //testObjectPool();
}
