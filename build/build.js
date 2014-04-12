/*
    Solution settings
*/

Solution.name           = "nixy_" + Make.CC
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
    var prj = Projects.create("nixycore_" + Make.CC)
    prj.type = "lib"
    prj.heads = "\
        'nixycore/general/plat_detect.h' \
        'nixycore/general/global_functions.h' \
        'nixycore/general/general.h' \
        'nixycore/preprocessor/pp_macros.h' \
        'nixycore/preprocessor/pp_count.h' \
        'nixycore/preprocessor/pp_arg.h' \
        'nixycore/preprocessor/pp_repeat.h' \
        'nixycore/preprocessor/pp_nest.h' \
        'nixycore/preprocessor/pp_mult.h' \
        'nixycore/preprocessor/preprocessor.h' \
        'nixycore/bugfix/noexcept.h' \
        'nixycore/bugfix/output.h' \
        'nixycore/bugfix/assert_detail.h' \
        'nixycore/bugfix/assert.h' \
        'nixycore/bugfix/trace.h' \
        'nixycore/bugfix/bugfix.h' \
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
        'nixycore/utility/type_cast.h' \
        'nixycore/utility/countof.h' \
        'nixycore/utility/operator.h' \
        'nixycore/utility/noncopyable.h' \
        'nixycore/utility/final.h' \
        'nixycore/utility/initialize.h' \
        'nixycore/utility/max_min.h' \
        'nixycore/utility/limit_of.h' \
        'nixycore/utility/safe_bool.h' \
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
        'nixycore/pattern/prototype.h' \
        'nixycore/pattern/singleton.h' \
        'nixycore/pattern/iterator.h' \
        'nixycore/pattern/trackable.h' \
        'nixycore/pattern/pattern.h' \
        'nixycore/time/tickcount.h' \
        'nixycore/time/stopwatch.h' \
        'nixycore/time/time.h' \
        'nixycore/random/rand_std.h' \
        'nixycore/random/rand_mt19937.h' \
        'nixycore/random/random.h' \
        'nixycore/delegate/function_traits.h' \
        'nixycore/delegate/functor.h' \
        'nixycore/delegate/bind.h' \
        'nixycore/delegate/delegate.h' \
        'nixycore/delegate/signal.h' \
        'nixycore/finalizer/scope_guard.h' \
        'nixycore/finalizer/ref_counter.h' \
        'nixycore/finalizer/holder.h' \
        'nixycore/finalizer/gc.h' \
        'nixycore/finalizer/finalizer.h' \
        'nixycore/memory/construct.h' \
        'nixycore/memory/alloc.h' \
        'nixycore/memory/std_alloc.h' \
        'nixycore/memory/fixed_pool.h' \
        'nixycore/memory/cache_pool.h' \
        'nixycore/memory/mem_pool.h' \
        'nixycore/memory/center_heap.h' \
        'nixycore/memory/mem_alloc.h' \
        'nixycore/memory/default_alloc.h' \
        'nixycore/memory/mem_leak.h' \
        'nixycore/memory/unfixed_pool.h' \
        'nixycore/memory/object_pool.h' \
        'nixycore/memory/pointer.h' \
        'nixycore/memory/mem_guard.h' \
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
        'nixycore/container/forward_list.h' \
        'nixycore/thread/barrier.h' \
        'nixycore/thread/interlocked.h' \
        'nixycore/thread/atomic.h' \
        'nixycore/thread/spin_lock.h' \
        'nixycore/thread/lock_guard.h' \
        'nixycore/thread/mutex.h' \
        'nixycore/thread/condition.h' \
        'nixycore/thread/semaphore.h' \
        'nixycore/thread/waiter.h' \
        'nixycore/thread/blocking_queue.h' \
        'nixycore/thread/tls_ptr.h' \
        'nixycore/thread/thread_ops.h' \
        'nixycore/thread/thread_model.h' \
        'nixycore/thread/thread_detail.h' \
        'nixycore/thread/thread_pool.h' \
        'nixycore/thread/async.h' \
        'nixycore/thread/thread.h' \
        'nixycore/string/transform.h' \
        'nixycore/string/string_detail.h' \
        'nixycore/string/string.h' \
        'nixycore/stream/printf_format.h' \
        'nixycore/stream/stream_wrap.h' \
        'nixycore/stream/stream_buffer.h' \
        'nixycore/stream/stream_detail.h' \
        'nixycore/stream/stream_ops.h' \
        'nixycore/stream/stream.h'"
}
/* project test */
{
    var prj = Projects.create("codecheck_" + Make.CC)
    prj.type = "console"
    if (Make.PLAT == "linux")
    {
        prj.libs = "-lpthread -lrt"
    }
    prj.heads = "\
        'codecheck/test_head.h' \
        'codecheck/test_preprocessor.h' \
        'codecheck/test_bugfix.h' \
        'codecheck/test_typemanip.h' \
        'codecheck/test_algorithm.h' \
        'codecheck/test_utility.h' \
        'codecheck/test_time.h' \
        'codecheck/test_random.h' \
        'codecheck/test_delegate.h' \
        'codecheck/test_finalizer.h' \
        'codecheck/test_thread.h' \
        'codecheck/test_memory.h' \
        'codecheck/test_string.h' \
        'codecheck/test_stream.h'"
    prj.sources = "\
        'codecheck/main.cpp' \
        'codecheck/dummy.cpp'"
}