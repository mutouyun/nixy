/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

//////////////////////////////////////////////////////////////////////////

typedef thread_ops::id_t        id_t;
typedef thread_ops::handle_t    handle_t;
typedef thread_ops::native_id_t native_id_t;
typedef thread_ops::proc_t      proc_t;

static handle_t create(proc_t proc, pvoid arg = 0, id_t* thr_id = 0)
{
    return thread_ops::create(proc, arg, thr_id);
}

static void exit(void)
{
    thread_ops::exit();
}

static native_id_t native_current_id(void)
{
    return thread_ops::native_current_id();
}

static id_t current_id(void)
{
    return thread_ops::current_id();
}

static id_t handle2id(handle_t hd)
{
    return thread_ops::handle2id(hd);
}

static bool join(handle_t hd)
{
    return thread_ops::join(hd);
}

static bool detach(handle_t hd)
{
    return thread_ops::detach(hd);
}

static void sleep(unsigned ms)
{
    thread_ops::sleep(ms);
}

static void yield(void)
{
    thread_ops::yield();
}

static void pause(unsigned k = 4)
{
    thread_ops::pause(k);
}

static unsigned hardware_concurrency(void)
{
    return thread_ops::hardware_concurrency();
}

//////////////////////////////////////////////////////////////////////////
