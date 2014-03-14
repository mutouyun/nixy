/*
    Solution settings
*/

Solution.name           = "nixy"
Solution.project_path   = "../"
Solution.include_path   = ""
Solution.tmp_path       = "!tmp/$(Configuration)/$(CC)"
Solution.out_path       = "!bin/$(Configuration)/$(CC)"

if (Make.PLAT == "win")
{
    Solution.CFLAGS = "-pipe -frtti -Wall -Wextra -fexceptions -march=nocona"
    Solution.LFLAGS = "-Wl,-s -Wl,-subsystem,console -mthreads"
}
else
{
    Solution.CFLAGS = "-pipe -frtti -Wall -Wextra -fexceptions -march=nocona"
    Solution.LFLAGS = "-Wl,-s"
}

/*
    Projects settings
*/

/* project nixycore */
{
    var prj = Projects.create("nixycore")
    prj.type = "lib"
    prj.heads = "\
        'nixycore/al/general/platdetect.hxx' \
        'nixycore/al/general/global_functions.hxx' \
        'nixycore/al/general/disable_warnings.hxx' \
        'nixycore/al/typemanip/private_is_pod.hxx' \
        'nixycore/al/typemanip/typeof.hxx' \
        'nixycore/al/typemanip/typeof_msvc.hxx' \
        'nixycore/al/algorithm/std_algorithm.hxx' \
        'nixycore/al/time/tickcount.hxx' \
        'nixycore/al/delegate/private_function_traits.hxx' \
        'nixycore/al/thread/barrier.hxx' \
        'nixycore/al/thread/barrier_gnuc.hxx' \
        'nixycore/al/thread/barrier_msvc.hxx' \
        'nixycore/al/thread/interlocked.hxx' \
        'nixycore/al/thread/interlocked_gnuc.hxx' \
        'nixycore/al/thread/interlocked_def_msvc.hxx' \
        'nixycore/al/thread/interlocked_msvc.hxx' \
        'nixycore/al/thread/mutex.hxx' \
        'nixycore/al/thread/mutex_linux.hxx' \
        'nixycore/al/thread/mutex_win.hxx' \
        'nixycore/al/thread/condition.hxx' \
        'nixycore/al/thread/condition_linux.hxx' \
        'nixycore/al/thread/condition_win.hxx' \
        'nixycore/al/thread/tlsptr.hxx' \
        'nixycore/al/thread/tlsptr_linux.hxx' \
        'nixycore/al/thread/tlsptr_win.hxx' \
        'nixycore/al/thread/threadops.hxx' \
        'nixycore/al/thread/threadops_linux.hxx' \
        'nixycore/al/thread/threadops_win.hxx' \
        'nixycore/al/string/transform.hxx' \
        'nixycore/al/string/stringops.hxx' \
        'nixycore/general/general.h' \
        'nixycore/preprocessor/ppmacros.h' \
        'nixycore/preprocessor/ppcount.h' \
        'nixycore/preprocessor/pparg.h' \
        'nixycore/preprocessor/pprepeat.h' \
        'nixycore/preprocessor/ppnest.h' \
        'nixycore/preprocessor/ppmult.h' \
        'nixycore/preprocessor/preprocessor.h' \
        'nixycore/typemanip/typedefs.h' \
        'nixycore/typemanip/typetools.h' \
        'nixycore/typemanip/typerelation.h' \
        'nixycore/typemanip/typelist.h' \
        'nixycore/typemanip/typeconcept.h' \
        'nixycore/typemanip/typequalifier.h' \
        'nixycore/typemanip/typebehavior.h' \
        'nixycore/typemanip/typedetect.h' \
        'nixycore/typemanip/typetraits.h' \
        'nixycore/typemanip/typeof.h' \
        'nixycore/typemanip/typemanip.h' \
        'nixycore/utility/addressof.h' \
        'nixycore/utility/typecast.h' \
        'nixycore/utility/countof.h' \
        'nixycore/utility/operator.h' \
        'nixycore/utility/noncopyable.h' \
        'nixycore/utility/final.h' \
        'nixycore/utility/initialize.h' \
        'nixycore/utility/maxmin.h' \
        'nixycore/utility/limitof.h' \
        'nixycore/utility/safebool.h' \
        'nixycore/utility/refer.h' \
        'nixycore/utility/rvalue.h' \
        'nixycore/utility/valid.h' \
        'nixycore/utility/tuple.h' \
        'nixycore/utility/utility.h' \
        'nixycore/algorithm/container.h' \
        'nixycore/algorithm/foreach.h' \
        'nixycore/algorithm/assign.h' \
        'nixycore/algorithm/series.h' \
        'nixycore/algorithm/algorithm.h' \
        'nixycore/pattern/stream.h' \
        'nixycore/pattern/prototype.h' \
        'nixycore/pattern/singleton.h' \
        'nixycore/pattern/iterator.h' \
        'nixycore/pattern/trackable.h' \
        'nixycore/pattern/pattern.h' \
        'nixycore/time/tickcount.h' \
        'nixycore/time/stopwatch.h' \
        'nixycore/time/time.h' \
        'nixycore/random/randstd.h' \
        'nixycore/random/randmt19937.h' \
        'nixycore/random/random.h' \
        'nixycore/delegate/functiontraits.h' \
        'nixycore/delegate/functor.h' \
        'nixycore/delegate/bind.h' \
        'nixycore/delegate/delegate.h' \
        'nixycore/delegate/signal.h' \
        'nixycore/bugfix/assert.h' \
        'nixycore/finalizer/scopeguard.h' \
        'nixycore/finalizer/refcounter.h' \
        'nixycore/finalizer/holder.h' \
        'nixycore/finalizer/gc.h' \
        'nixycore/finalizer/finalizer.h' \
        'nixycore/memory/construct.h' \
        'nixycore/memory/stdalloc.h' \
        'nixycore/memory/fixedpool.h' \
        'nixycore/memory/centerpool.h' \
        'nixycore/memory/mempool.h' \
        'nixycore/memory/memalloc.h' \
        'nixycore/memory/alloc.h' \
        'nixycore/memory/unfixedpool.h' \
        'nixycore/memory/objectpool.h' \
        'nixycore/memory/pointer.h' \
        'nixycore/memory/memguard.h' \
        'nixycore/memory/memory.h' \
        'nixycore/container/any.h' \
        'nixycore/container/array.h' \
        'nixycore/container/container.h' \
        'nixycore/container/deque.h' \
        'nixycore/container/list.h' \
        'nixycore/container/map.h' \
        'nixycore/container/priority.h' \
        'nixycore/container/queue.h' \
        'nixycore/container/set.h' \
        'nixycore/container/stack.h' \
        'nixycore/container/vector.h' \
        'nixycore/thread/barrier.h' \
        'nixycore/thread/interlocked.h' \
        'nixycore/thread/atomic.h' \
        'nixycore/thread/spinlock.h' \
        'nixycore/thread/lockguard.h' \
        'nixycore/thread/mutex.h' \
        'nixycore/thread/condition.h' \
        'nixycore/thread/semaphore.h' \
        'nixycore/thread/waiter.h' \
        'nixycore/thread/blockingqueue.h' \
        'nixycore/thread/tlsptr.h' \
        'nixycore/thread/threadops.h' \
        'nixycore/thread/threadmodel.h' \
        'nixycore/thread/threadops_define.hxx' \
        'nixycore/thread/threaddetail.h' \
        'nixycore/thread/threadpool.h' \
        'nixycore/thread/async.h' \
        'nixycore/thread/thread.h' \
        'nixycore/string/transform.h' \
        'nixycore/string/stringops_define.hxx' \
        'nixycore/string/stringdetail.h' \
        'nixycore/string/string.h'"
}
/* project test */
{
    var prj = Projects.create("codecheck")
    prj.type = "console"
    if (Make.PLAT == "linux")
    {
        prj.libs = "-lpthread -lrt"
    }
    prj.heads = "\
        'codecheck/test_head.h' \
        'codecheck/test_preprocessor.h' \
        'codecheck/test_typemanip.h' \
        'codecheck/test_algorithm.h' \
        'codecheck/test_utility.h' \
        'codecheck/test_time.h' \
        'codecheck/test_delegate.h' \
        'codecheck/test_finalizer.h' \
        'codecheck/test_thread.h' \
        'codecheck/test_memory.h' \
        'codecheck/test_string.h'"
    prj.sources = "\
        'codecheck/main.cpp' \
        'codecheck/dummy.cpp'"
}