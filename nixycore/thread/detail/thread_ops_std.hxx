/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#include <thread>
#include <chrono> // std::chrono::milliseconds

#if defined(NX_OS_WIN) && !defined(NX_OS_WINCE)
#include <process.h>
#elif defined(NX_OS_LINUX)
#include <pthread.h>
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_OS_WIN
namespace private_tls_ptr
{
    inline void __cdecl onThreadExit(void);
}
#endif

namespace thread_ops
{
    typedef std::thread::id id_t;
    typedef std::thread*    handle_t;
#if defined(NX_OS_WIN)
    typedef DWORD           native_id_t;
#elif defined(NX_OS_LINUX)
    typedef pthread_t       native_id_t;
#else
    typedef size_t          native_id_t;
#endif

    inline void clear_id(id_t& id)
    {
        id = id_t();
    }

    inline void clear_hd(handle_t& hd)
    {
        hd = nx::nulptr;
    }

#define NX_THREAD_PROC(name, ...) unsigned (name)(void* __VA_ARGS__)
    typedef NX_THREAD_PROC(*proc_t);

#ifdef NX_OS_WIN
    namespace private_
    {
        struct thread_data
        {
            proc_t proc_;
            void* args_;
            thread_data(proc_t proc, void* args)
                : proc_(proc), args_(args)
            {}
        };

        struct thread_data_ptr
        {
            thread_data* ptr_;

            thread_data_ptr(pvoid p)
                : ptr_(static_cast<thread_data*>(p))
            {}

            ~thread_data_ptr(void)
            { if (ptr_) delete ptr_; }

            void dismiss(void)
            { ptr_ = 0; }

            thread_data* operator->(void) const
            { return ptr_; }
        };

        static NX_THREAD_PROC(onThreadProxyProc, args)
        {
            thread_data_ptr ptr(args);
            unsigned ret = ptr->proc_(ptr->args_);
            private_tls_ptr::onThreadExit();
            return ret;
        }
    }
#endif

    inline handle_t create(proc_t proc, pvoid arg = 0, id_t* thr_id = 0)
    {
#   ifdef NX_OS_WIN
        using namespace private_;
        thread_data_ptr ptr(new thread_data(proc, arg));
        handle_t hd = new std::thread(&onThreadProxyProc, ptr.ptr_);
        if (!hd) return 0;
        ptr.dismiss();
#   else
        handle_t hd = new std::thread(proc, arg);
        if (!hd) return 0;
#   endif
        if (thr_id) (*thr_id) = hd->get_id();
        return hd;
    }

    inline void exit(void)
    {
#   if defined(NX_OS_WINCE)
        ExitThread(0);
#   elif defined(NX_OS_WIN)
        _endthread();
#   elif defined(NX_OS_LINUX)
        pthread_exit(0);
#   else
        nx_assert(false); // Not support
#   endif
    }

    inline native_id_t native_current_id(void)
    {
#   if defined(NX_OS_WIN)
        return GetCurrentThreadId();
#   elif defined(NX_OS_LINUX)
        return pthread_self();
#   else
        return 0; // Not support
#   endif
    }

    inline id_t current_id(void)
    {
        return std::this_thread::get_id();
    }

    inline id_t handle2id(handle_t hd)
    {
        return hd ? hd->get_id() : id_t();
    }

    inline bool join(handle_t hd)
    {
        if (hd)
        {
            hd->join();
            delete hd;
            return true;
        }
        return false;
    }

    inline bool detach(handle_t hd)
    {
        if (hd)
        {
            hd->detach();
            delete hd;
            return true;
        }
        return false;
    }

    inline void sleep(unsigned ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    inline void yield(void)
    {
        std::this_thread::yield();
    }

    inline unsigned hardware_concurrency(void)
    {
        return std::thread::hardware_concurrency();
    }
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
