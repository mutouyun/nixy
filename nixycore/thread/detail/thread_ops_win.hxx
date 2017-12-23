/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#include <tlhelp32.h>
#ifndef NX_OS_WINCE
#include <process.h>
#endif/*NX_OS_WINCE*/

//////////////////////////////////////////////////////////////////////////
NX_BEG
namespace private_tls_ptr {
inline void __cdecl onThreadExit(void);
}
namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

typedef DWORD  id_t;
typedef HANDLE handle_t;
typedef DWORD  native_id_t;

inline void clear_id(id_t& id)
{
    id = 0;
}

inline void clear_hd(handle_t& hd)
{
    hd = NULL; /* Not INVALID_HANDLE_VALUE */
}

#define NX_THREAD_PROC(name, ...) unsigned (__stdcall name)(void* __VA_ARGS__)
typedef NX_THREAD_PROC(*proc_t);

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

#   ifdef NX_OS_WINCE
    static DWORD WINAPI onThreadProxyProc(LPVOID args)
#   else
    static NX_THREAD_PROC(onThreadProxyProc, args)
#   endif/*NX_OS_WINCE*/
    {
        thread_data_ptr ptr(args);
        unsigned ret = ptr->proc_(ptr->args_);
        private_tls_ptr::onThreadExit();
        return ret;
    }

    inline handle_t begin_thread(void* security, unsigned stack_size,
                                 unsigned (__stdcall* start_address)(void*), void* arg_list,
                                 unsigned init_flag, unsigned* thrd_addr)
    {
        thread_data_ptr ptr(new thread_data(start_address, arg_list));

#   ifdef NX_OS_WINCE
        DWORD thread_id;
        HANDLE hd = CreateThread((LPSECURITY_ATTRIBUTES)(security), stack_size,
                                 onThreadProxyProc, ptr.ptr_, init_flag, &thread_id);
#   else
        HANDLE hd = (HANDLE)_beginthreadex(security, stack_size,
                                 onThreadProxyProc, ptr.ptr_, init_flag, thrd_addr);
#   endif/*NX_OS_WINCE*/

        if (hd == NULL) return 0;

#   ifdef NX_OS_WINCE
        if (thrd_addr) (*thrd_addr) = thread_id;
#   endif/*NX_OS_WINCE*/

        ptr.dismiss();
        return (handle_t)(hd);
    }

    inline void end_thread(void)
    {
#   ifdef NX_OS_WINCE
        ExitThread(0);
#   else
        _endthread();
#   endif/*NX_OS_WINCE*/
    }
}

inline handle_t create(proc_t proc, pvoid arg = 0, id_t* thr_id = 0)
{
    return private_::begin_thread(NULL, 0, proc, arg, 0, (unsigned*)(thr_id));
}

inline void exit(void)
{
    private_::end_thread();
}

inline native_id_t native_current_id(void)
{
    return GetCurrentThreadId();
}

inline id_t current_id(void)
{
    return GetCurrentThreadId();
}

inline id_t handle2id(handle_t hd)
{
#if defined(NX_OS_WINCE)
    return (id_t)hd; /* T.B.D */
#elif (WINVER >= 0x0502)
    return GetThreadId(hd);
#else
    if (hd == GetCurrentThread()) return current_id();
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) return 0;
    THREADENTRY32 te32 = THREADENTRY32();
    te32.dwSize = sizeof(te32);
    if (Thread32First(hThreadSnap, &te32) == FALSE) return 0;
    DWORD dwProcessId = GetCurrentProcessId();
    while(Thread32Next(hThreadSnap, &te32))
    {
        if (te32.th32OwnerProcessID == dwProcessId)
        {
            HANDLE hTempThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
            if (hTempThread == hd)
            {
                CloseHandle(hTempThread);
                CloseHandle(hThreadSnap);
                return te32.th32ThreadID;
            }
            else
                CloseHandle(hTempThread);
        }
    }
    return 0;
#endif
}

inline bool join(handle_t hd)
{
    if (WaitForSingleObject(hd, INFINITE) == WAIT_OBJECT_0)
        return !!CloseHandle(hd);
    return false;
}

inline bool detach(handle_t hd)
{
    return !!CloseHandle(hd);
}

inline void sleep(unsigned ms)
{
    Sleep(ms);
}

inline void yield(void)
{
#if defined(NX_OS_WINCE)
    sleep(0);
#else
    SwitchToThread();
#endif
}

inline unsigned hardware_concurrency(void)
{
    static unsigned cpu_count = 0;
    if (cpu_count > 0) return cpu_count;
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    cpu_count = static_cast<unsigned>(si.dwNumberOfProcessors);
    if (cpu_count < 1) cpu_count = 1; // fail-safe
    return cpu_count;
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
