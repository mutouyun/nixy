/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

typedef thread_ops::id_t     id_t;
typedef thread_ops::handle_t handle_t;
typedef thread_ops::proc_t   proc_t;

static handle_t create(proc_t proc, pvoid arg = 0, id_t* thr_id = 0)
{
    return thread_ops::create(proc, arg, thr_id);
}

static handle_t current_handle(void)
{
    return thread_ops::current_handle();
}

static id_t current_id(void)
{
    return thread_ops::current_id();
}

static handle_t id2handle(id_t id)
{
    return thread_ops::id2handle(id);
}

static id_t handle2id(handle_t hd)
{
    return thread_ops::handle2id(hd);
}

static bool cancel(handle_t hd)
{
    return thread_ops::cancel(hd);
}

static bool join(handle_t hd)
{
    return thread_ops::join(hd);
}

static bool detach(handle_t hd)
{
    return thread_ops::detach(hd);
}

static void exit(void)
{
    return thread_ops::exit();
}

static void sleep(unsigned ms)
{
    return thread_ops::sleep(ms);
}

static void yield(void)
{
    return thread_ops::yield();
}

static void pause(unsigned k = 4)
{
    return thread_ops::pause(k);
}

//////////////////////////////////////////////////////////////////////////
