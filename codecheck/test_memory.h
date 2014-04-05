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
    const size_t TestCont = 100;
    const size_t TestLast = 100;
#elif NDEBUG
    const size_t TestCont = 100000;
    const size_t TestLast = 1000;
#else
    const size_t TestCont = 1000;
    const size_t TestLast = 1000;
#endif
    const size_t TestSMin = 1;
    const size_t TestSMax = /*4*//*16*//*32*/256/*1024*//*65536*//*1024 * 1024*/;

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
        for(size_t i = 0; i < TestLast; ++i)
            rdm.roll(size[i]);

        rdm.range(0, TestLast);
        for(size_t i = 0; i < TestLast; ++i)
            rdm.roll(indx[i]);
    }

#define TEST_MEMPOOL(alloc_name, test_last, start_n) do { \
    for(size_t i = 0; i < (TestCont / 2); ++i) \
    { \
        alloc_name alc; \
        for(int x = 0; x < 2; ++x) \
        for(size_t n = 0, i = (start_n * test_last); n < test_last; ++n, ++i) \
        { \
            test_t& p = test[indx[i]]; \
            if (p.p_) \
            { \
                alc.free(p.p_, p.s_); \
                p.s_ = 0; \
                p.p_ = 0; \
            } \
            else \
            { \
                p.p_ = alc.alloc(size[i]); \
                p.s_ = size[i]; \
            } \
        } \
    } } while(0)

    template <typename A>
    void start(const char* out)
    {
        test_t test[TestLast];
        strout << out;

        sw.start();
        TEST_MEMPOOL(A, TestLast, 0);
        strout << sw.value() * 1000 << " ms" << endl;
    }

    struct system_alloc
    {
        void* alloc(size_t size)
        {
            return ::malloc(size);
        }
        void free(void* p, size_t /*size*/)
        {
            ::free(p);
        }
    };

    struct mempool_alloc
    {
        nx::use::alloc_pool::mem_pool_t pool_;
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

//    start<unfixed_alloc>("Start for nx::unfixed_pool...\t");
    //start<mempool_alloc>("Start for nx::mem_pool...\t");
    start<mem_alloc>    ("Start for nx::alloc/free...\t");
#ifndef NO_TEST_NEDMALLOC
    start<ned_alloc>    ("Start for ned_alloc/free...\t");
#endif
    start<system_alloc> ("Start for malloc...\t\t");
}

//////////////////////////////////////////////////////////////////////////

namespace test_thread_alloc
{
    nx::thread_ops::handle_t hd[10] = {0};
    void* alc_ptr_ = 0;

    const size_t TestLast_Thread = test_mempool::TestLast / nx_countof(hd);

#define THREAD_ALLOC(alloc_name) \
    NX_THREAD_PROC(proc_##alloc_name, xx) \
    { \
        using namespace test_mempool; \
        alloc_name& alc = *(alloc_name*)alc_ptr_; \
        size_t start_n = reinterpret_cast<size_t>(xx); \
        test_t test[TestLast]; \
        TEST_MEMPOOL((void), TestLast_Thread, start_n); \
        return 0; \
    }
    THREAD_ALLOC(system_alloc)
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
        alc_ptr_ = &alc; \
        strout << out; \
        sw.start(); \
        nx_foreach(i, nx_countof(hd)) hd[i] = nx::thread_ops::create(proc_##alloc_name, (void*)i); \
        nx_foreach(i, nx_countof(hd)) nx::thread_ops::join(hd[i]); \
        strout << sw.value() * 1000 << " ms" << endl; \
    }
    START_ALLOC(system_alloc)
    START_ALLOC(mem_alloc)
#ifndef NO_TEST_NEDMALLOC
    START_ALLOC(ned_alloc)
#endif
#undef START_ALLOC
}

void testThreadAlloc(void)
{
    TEST_CASE();

    using namespace test_mempool;
    using namespace test_thread_alloc;

    init(nx_countof(hd));

    start_mem_alloc   ("Start for nx::alloc/free...\t");
#ifndef NO_TEST_NEDMALLOC
    start_ned_alloc   ("Start for ned_alloc/free...\t");
#endif
    start_system_alloc("Start for malloc...\t\t");
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
    testThreadAlloc();
    //testMemGuard();
    //testPointer();
    //testObjectPool();
}
