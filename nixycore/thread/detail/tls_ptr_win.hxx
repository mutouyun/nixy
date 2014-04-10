/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#include "nixycore/pattern/trackable.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_tls_ptr {
//////////////////////////////////////////////////////////////////////////

#ifndef TLS_OUT_OF_INDEXES  // wince does not define the TLS_OUT_OF_INDEXES constant
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#endif/*TLS_OUT_OF_INDEXES*/

/*
    The list node for saving destructors
*/

class tls_data : public trackable<tls_data>
{
public:
    void (*destructor_)(void*);

private:
    DWORD index_;

public:
    tls_data(void (*destructor)(void*))
        : destructor_(destructor)
        , index_(TlsAlloc())
    {}

    ~tls_data(void)
    {
        if (destructor_)
        {
            void* p = get();
            if (p) destructor_(p);
        }
        // free the tls index
        TlsFree(index_);
    }

public:
    bool set(LPVOID ptr)
    {
        nx_assert(index_ != TLS_OUT_OF_INDEXES);
        return !!TlsSetValue(index_, ptr);
    }

    LPVOID get(void) const
    {
        nx_assert(index_ != TLS_OUT_OF_INDEXES);
        return TlsGetValue(index_);
    }
};

/*
    Call destructor on thread exit
*/

inline void __cdecl onThreadExit(void)
{
    tls_data* ite = tls_data::track();
    while(ite)
    {
        if (ite->destructor_)
        {
            void* p = ite->get();
            if (p) ite->destructor_(p);
        }
        ite = ite->next_;
    }
}

/*
    The detail of tls_ptr
*/

template <typename T>
class detail : tls_data
{
public:
    detail(void (*destructor)(void*))
        : tls_data(destructor)
    {}

public:
    bool set(T* ptr)
    {
        return tls_data::set((LPVOID)ptr);
    }

    T* get(void) const
    {
        return (T*)tls_data::get();
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
