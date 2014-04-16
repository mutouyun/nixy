#pragma once

#include "nixycore/memory/memory.h"
#include "nixycore/time/stopwatch.h"
#include "nixycore/random/random.h"
#include "nixycore/thread/thread.h"

#if (NX_CC_MSVC == 1400) || defined(NX_OS_WINCE)
#define NO_TEST_NEDMALLOC
#else
//#define NO_TEST_NEDMALLOC
#endif

#define NO_TEST_JEMALLOC
#define NO_TEST_TCMALLOC

#ifndef NO_TEST_NEDMALLOC
#ifdef NX_CC_GNUC
#   pragma GCC diagnostic ignored "-Wunused-value"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#   ifdef NX_CC_CLANG
#   pragma GCC diagnostic ignored "-Wself-assign"
#   endif
#endif
#include "nedmalloc/nedmalloc.c"
#endif

#ifndef NO_TEST_JEMALLOC
#ifdef NX_CC_MSVC
#   pragma comment(lib, "jemalloc.lib")
#endif
#   if defined(NX_OS_WIN)
    extern "C" __declspec(dllimport) void* je_malloc(size_t size);
    extern "C" __declspec(dllimport) void  je_free(void *ptr);
#   else
    extern "C" void* je_malloc(size_t size);
    extern "C" void  je_free(void *ptr);
#   endif
#endif

#ifndef NO_TEST_TCMALLOC
#ifdef NX_CC_MSVC
#   pragma comment(lib, "libtcmalloc_minimal.lib")
#endif
#   if defined(NX_OS_WIN)
    extern "C" __declspec(dllimport) void* tc_malloc(size_t size);
    extern "C" __declspec(dllimport) void  tc_free(void* ptr);
#   else
    extern "C" void* tc_malloc(size_t size);
    extern "C" void  tc_free(void* ptr);
#   endif
#endif

//////////////////////////////////////////////////////////////////////////

namespace test_mempool
{
#if NDEBUG
#   ifndef NX_OS_WINCE
    const size_t TestCont = 100000;
    const size_t TestLast = 1000;
#   else
    const size_t TestCont = 10000;
    const size_t TestLast = 100;
#   endif
#else
#   ifndef NX_OS_WINCE
    const size_t TestCont = 1000;
    const size_t TestLast = 1000;
#   else
    const size_t TestCont = 100;
    const size_t TestLast = 100;
#   endif
#endif
    const size_t TestSMin = 1/*256*/;
    const size_t TestSMax = /*4*//*16*//*32*/256/*1024*//*4096*//*65536*//*1024 * 1024*/;

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

#ifndef NO_TEST_JEMALLOC
    struct je_alloc
    {
        void* alloc(size_t size)
        {
            return je_malloc(size);
        }
        void free(void* p, size_t /*size*/)
        {
            je_free(p);
        }
    };
#endif

#ifndef NO_TEST_TCMALLOC
    struct tc_alloc
    {
        void* alloc(size_t size)
        {
            return tc_malloc(size);
        }
        void free(void* p, size_t /*size*/)
        {
            tc_free(p);
        }
    };
#endif

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
}

void testMemPool(void)
{
    TEST_CASE();

    using namespace test_mempool;

    init();

    //start<unfixed_alloc>("Start for nx::unfixed_pool...\t");
    //start<mempool_alloc>("Start for nx::mem_pool...\t");
    start<mem_alloc>   ("Start for nx::alloc/free...\t");
#ifndef NO_TEST_NEDMALLOC
    start<ned_alloc>   ("Start for ned_alloc/free...\t");
#endif
#ifndef NO_TEST_JEMALLOC
    start<je_alloc>    ("Start for jemalloc...\t\t");
#endif
#ifndef NO_TEST_TCMALLOC
    start<tc_alloc>    ("Start for tcmalloc...\t\t");
#else
    start<system_alloc>("Start for malloc...\t\t");
#endif
}

//////////////////////////////////////////////////////////////////////////

namespace test_thread_alloc
{
    nx::thread_ops::handle_t hd[10] = {0};
    void* alc_ptr_ = 0;

    const size_t TestLast_Thread = test_mempool::TestLast / nx_countof(hd);

#define THREAD_ALLOC_TEST(alloc_name) \
    NX_THREAD_PROC(proc_##alloc_name, xx) \
    { \
        using namespace test_mempool; \
        alloc_name& alc = *(alloc_name*)alc_ptr_; \
        size_t start_n = reinterpret_cast<size_t>(xx); \
        test_t test[TestLast]; \
        TEST_MEMPOOL((void), TestLast_Thread, start_n); \
        return 0; \
    } \
    void start_##alloc_name(const char* out) \
    { \
        using namespace test_mempool; \
        alloc_name alc; \
        alc_ptr_ = &alc; \
        strout << out; \
        sw.start(); \
        for(size_t i = 0; i < nx_countof(hd); ++i) hd[i] = nx::thread_ops::create(proc_##alloc_name, (void*)i); \
        for(size_t i = 0; i < nx_countof(hd); ++i) nx::thread_ops::join(hd[i]); \
        strout << sw.value() * 1000 << " ms" << endl; \
    }
    THREAD_ALLOC_TEST(mem_alloc)
#ifndef NO_TEST_NEDMALLOC
    THREAD_ALLOC_TEST(ned_alloc)
#endif
#ifndef NO_TEST_JEMALLOC
    THREAD_ALLOC_TEST(je_alloc)
#endif
#ifndef NO_TEST_TCMALLOC
    THREAD_ALLOC_TEST(tc_alloc)
#endif
    THREAD_ALLOC_TEST(system_alloc)
#undef THREAD_ALLOC_TEST
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
#ifndef NO_TEST_JEMALLOC
    start_je_alloc    ("Start for jemalloc...\t\t");
#endif
#ifndef NO_TEST_TCMALLOC
    start_tc_alloc    ("Start for tcmalloc...\t\t");
#else
    start_system_alloc("Start for malloc...\t\t");
#endif
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

void testMemLeak(void)
{
    TEST_CASE();

    using namespace test_memguard;

    nx_alloc(12);
    nx_alloc(10);
    nx_free(nx_alloc<A>());
    nx_alloc<A>(123);
    /*nx_free*/(nx_alloc<A[10]>(321));
}

//////////////////////////////////////////////////////////////////////////

void testMemory(void)
{
    TEST_FUNCTION();

//    testMemPool();
//    testThreadAlloc();
    //testMemGuard();
    testPointer();
    //testObjectPool();
    testMemLeak();
}
