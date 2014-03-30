#pragma once

#include "nixycore/thread/thread.h"
#include "nixycore/random/random.h"

//////////////////////////////////////////////////////////////////////////

void testInterlocked(void)
{
    TEST_CASE();

    long a = 3, b = 2, c = 2;
    c = nx::use::interlocked_mt::compare_exchange(b, -a, c);
    strout << (int)a << " " << (int)b << " " << (int)c << endl;
}

//////////////////////////////////////////////////////////////////////////

void testAtomic(void)
{
    TEST_CASE();

    nx::atomic<bool> a;
    strout << a << endl;
    if (!a) a = true;
    strout << a << endl;
}

//////////////////////////////////////////////////////////////////////////

namespace test_threadops
{
    nx::random<> rdm(10, 200);

    NX_THREAD_PROC(proc)
    {
        rdm.srand(nx::thread_ops::current_id());
        nx_foreach(NX_UNUSED n, 10)
        {
            unsigned st = 0;
            {
                nx_lock_sole(nx::spin_lock);
                strout << nx::thread_ops::current_id() << "\t";
                nx_foreach(i, 10) strout << i << " ";
                strout << (st = rdm.roll<unsigned>()) << endl;
            }
            nx::thread_ops::sleep(st);
        }
        return 0;
    }
}

void testThreadOps(void)
{
    TEST_CASE();

    using namespace test_threadops;

    nx::thread_ops::handle_t hd[10] = {0};
    nx_foreach(i, nx_countof(hd)) hd[i] = nx::thread_ops::create(proc);
    nx_foreach(i, nx_countof(hd)) nx::thread_ops::join(hd[i]);
}

//////////////////////////////////////////////////////////////////////////

#include "nixycore/container/queue.h"

namespace test_condition
{
    nx::random<>   rdm(1, 10);
    nx::queue<int> codes;
    nx::mutex      codes_lock;
    nx::condition  codes_cond(codes_lock);
    bool           is_done = false;

    NX_THREAD_PROC(workerfunc, idptr)
    {
        unsigned st = 0, id = nx::horrible_cast<unsigned>(idptr);

        // print a starting message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[worker " << id << "]\trunning..." << endl;
            st = rdm.roll<unsigned>() * 1000;
        }

        // simulate work
        nx::thread_ops::sleep(st);

        // simulate error
        int errorcode = id * 100 + 1;
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[worker " << id << "]\tan error occurred: " << errorcode << endl;
        }

        // notify error to be logged
        {
            nx_lock_scope(codes_lock);
            codes.push(errorcode);
            codes_cond.notify();
        }

        return 0;
    }

    NX_THREAD_PROC(loggerfunc, idptr)
    {
        unsigned id = nx::horrible_cast<unsigned>(idptr);

        // print a starting message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[logger-" << id << "]\trunning..." << endl;
        }

        // loop until end is signaled
        while(!is_done)
        {
            nx_lock_scope(codes_lock);

            // wait for 1 secs
            while (!is_done && codes.empty()) // used to avoid spurious wakeups
            {
                codes_cond.wait(1000);
            }

            // if there are error codes in the queue process them
            while (!codes.empty())
            {
                nx_lock_sole(nx::spin_lock);
                strout << "[logger-" << id << "]\tprocessing error:  " << codes.front() << endl;
                codes.pop();
            }
        }

        // print a ending message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[logger-" << id << "]\tending..." << endl;
        }
        return 0;
    }
}

void testCondition(void)
{
    TEST_CASE();

    using namespace test_condition;

    // start the logger threads
    nx::thread_ops::handle_t lg[2] = {0};
    nx_foreach(i, nx_countof(lg)) lg[i] = nx::thread_ops::create(loggerfunc, (nx::pvoid)(i + 1));

    // start the working threads
    nx::thread_ops::handle_t hd[9] = {0};
    nx_foreach(i, nx_countof(hd)) hd[i] = nx::thread_ops::create(workerfunc, (nx::pvoid)(i + 1));
    nx_foreach(i, nx_countof(hd)) nx::thread_ops::join(hd[i]);

    // notify the loggers to finish and wait for it
    is_done = true;
    nx_foreach(i, nx_countof(lg)) nx::thread_ops::join(lg[i]);
}

//////////////////////////////////////////////////////////////////////////

namespace test_semaphore
{
    nx::random<>  rdm(100, 2000);
    bool          warehouse[10] = {false};
    int           counter = 0;
    nx::spin_lock warehouse_lock;
    nx::semaphore producer_sema(nx_countof(warehouse));
    nx::semaphore consumer_sema;
    bool          is_done = false;

    NX_THREAD_PROC(producer, idptr)
    {
        unsigned id = nx::horrible_cast<unsigned>(idptr);

        // print a starting message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[producer " << id << "]\trunning..." << endl;
        }

        // loop until end is signaled
        while(!is_done)
        {
            producer_sema.wait();

            // simulate work
            nx::thread_ops::sleep(1000);

            // put the product into warehouse
            nx_lock_scope(warehouse_lock);
            {
                nx_lock_sole(nx::spin_lock);
                strout << "[producer " << id << "]\tputting product No." << counter << endl;
            }
            warehouse[counter ++] = true;
            consumer_sema.post();
        }

        return 0;
    }

    NX_THREAD_PROC(consumer, idptr)
    {
        unsigned st = 0, id = nx::horrible_cast<unsigned>(idptr);

        // print a starting message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[consumer-" << id << "]\trunning..." << endl;
        }

        // loop until end is signaled
        while(!is_done)
        {
            consumer_sema.wait();

            // simulate work
            {
                nx_lock_sole(nx::spin_lock);
                st = rdm.roll<unsigned>();
            }
            nx::thread_ops::sleep(st);

            // get a product from warehouse
            nx_lock_scope(warehouse_lock);
            warehouse[-- counter] = false;
            {
                nx_lock_sole(nx::spin_lock);
                strout << "[consumer-" << id << "]\tget product No." << counter << endl;
            }
            producer_sema.post();
        }

        return 0;
    }
}

void testSemaphore(void)
{
    TEST_CASE();

    using namespace test_semaphore;

    // start the producer threads
    nx::thread_ops::handle_t pd[2] = {0};
    nx_foreach(i, nx_countof(pd)) pd[i] = nx::thread_ops::create(producer, (nx::pvoid)(i + 1));

    nx::thread_ops::sleep(10000);

    // start the consumer threads
    nx::thread_ops::handle_t cs[10] = {0};
    nx_foreach(i, nx_countof(cs)) cs[i] = nx::thread_ops::create(consumer, (nx::pvoid)(i + 1));

    // notify the producers and consumers to finish and wait for it
    //is_done = true;
    nx_foreach(i, nx_countof(cs)) nx::thread_ops::join(cs[i]);
    nx_foreach(i, nx_countof(pd)) nx::thread_ops::join(pd[i]);
}

//////////////////////////////////////////////////////////////////////////

namespace test_waiter
{
    nx::waiter consumer_waiter;

    NX_THREAD_PROC(test_proc, idptr)
    {
        unsigned id = nx::horrible_cast<unsigned>(idptr);

        // print a starting message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[tester " << id << "]\tis waiting..." << endl;
        }

        consumer_waiter.wait();

        // print a ending message
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[tester " << id << "]\tis ending..." << endl;
        }

        return 0;
    }

}

void testWaiter(void)
{
    TEST_CASE();

    using namespace test_waiter;

    // start the test threads
    nx::thread_ops::handle_t pd[10] = {0};
    nx_foreach(i, nx_countof(pd)) pd[i] = nx::thread_ops::create(test_proc, (nx::pvoid)(i + 1));

    // test for notify
    nx::thread_ops::sleep(1000);
    strout << endl << "test for notify" << endl;
    for(int i = 0; i < 3; ++i)
    {
        nx::thread_ops::sleep(1000);
        consumer_waiter.notify();
    }

    // test for broadcast
    nx::thread_ops::sleep(1000);
    strout << endl << "test for broadcast" << endl;
    nx::thread_ops::sleep(1000);
    consumer_waiter.broadcast();

    nx_foreach(i, nx_countof(pd)) nx::thread_ops::join(pd[i]);
}

//////////////////////////////////////////////////////////////////////////

namespace test_tlsptr
{
    void dest(void* p)
    {
        strout << "dest " << *(nx::ulong*)p << endl;
        delete (nx::ulong*)p;
    }

    nx::tls_ptr<nx::ulong> intptr(dest);

    NX_THREAD_PROC(proc)
    {
        intptr.set(new nx::ulong(0));
        (*intptr) = nx::thread_ops::current_id();
        {
            nx_lock_sole(nx::spin_lock);
            strout << "[proc " << (*intptr) << "]\tis running..." << endl;
        }
        return 0;
    }
}

void testTlsPtr(void)
{
    TEST_CASE();

    using namespace test_tlsptr;

    intptr.set(new nx::ulong(0));

    // start the working threads
    nx::thread_ops::handle_t hd[9] = {0};
    nx_foreach(i, nx_countof(hd)) hd[i] = nx::thread_ops::create(proc);
    nx_foreach(i, nx_countof(hd)) nx::thread_ops::join(hd[i]);

    strout << "[main " << (*intptr) << "]\tis running..." << endl;
}

//////////////////////////////////////////////////////////////////////////

#include "nixycore/memory/pointer.h"

namespace test_threaddetail
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

    void hello_world(void)
    {
        strout << endl  << "Hello World!" << endl << endl;
    }

    void thread_proc_1(nx::pointer<A> pa)
    {
        nx::thread::sleep(1000);
        strout << NX__FUNCTION__ << " " << pa->a_ << endl;
    }

    void* thread_proc_2(int)
    {
        strout << NX__FUNCTION__ << endl;

        nx::pointer<A> pa = nx::alloc<A>(2);
        nx::thread thr(&thread_proc_1, pa);

        return 0;
    }
}

void testThreadDetail(void)
{
    TEST_CASE();

    using namespace test_threaddetail;

    nx::thread NX_UNUSED thr1;
    nx::thread NX_UNUSED thr2(&thread_proc_2, 123);

    thr1.start();
    thr1.post(&hello_world);
    thr1.post(nx::none); // exit this thread
}

//////////////////////////////////////////////////////////////////////////

namespace test_threadpool
{
    void test_proc(int i)
    {
        {
            nx_lock_sole(nx::spin_lock);
            strout << "Test thread: " << i << endl;
        }
        nx::thread::sleep(1000);
        {
            nx_lock_sole(nx::spin_lock);
            strout << "Test thread: " << i << " Finish..." << endl;
        }
    }
}

void testThreadPool(void)
{
    TEST_CASE();

    using namespace test_threadpool;

    nx::thread_pool pool(0, 10);
    for(int i = 0; i < 10; ++i)
        pool.put(&test_proc, i);

    pool.wait_finish();
}

//////////////////////////////////////////////////////////////////////////

namespace test_async
{
    int test_1(int total)
    {
        int r = 0;
        for(int i = 1; i <= total; ++i)
            r += i;
        return r;
    }

    int test_2(int total)
    {
        int r = 1;
        for(int i = 1; i <= total; ++i)
            r *= i;
        return r;
    }

    nx::rvalue<int> test_3(void)
    {
        int r = 0;
        for(int i = 1; i <= 100; ++i)
            r += (i & 1) ? i : -i;
        return r;
    }
}

void testAsync(void)
{
    TEST_CASE();

    using namespace test_async;

    nx::task<int> t1 = nx::async(&test_1, 100);
    nx::task<int> t2 = nx::async(&test_2, 10);
    nx::task<nx::rvalue<int> > t3 = nx::async(&test_3);

    strout << "Check task result: " << t1.result()
                             << " " << t2.result()
                             << " " << t3.result() << endl;
}

//////////////////////////////////////////////////////////////////////////

void testThread(void)
{
    TEST_FUNCTION();

    //testInterlocked();
    //testAtomic();
    //testThreadOps();
    //testCondition();
    //testSemaphore();
    //testWaiter();
    //testTlsPtr();
    //testThreadDetail();
    //testThreadPool();
    testAsync();
}
