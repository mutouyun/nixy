/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_mutex {
//////////////////////////////////////////////////////////////////////////

class detail
{
protected:
    CRITICAL_SECTION mx_;

public:
    detail(void)
    {
#   if defined(NX_OS_WINCE)
        InitializeCriticalSection(&mx_);
#   else
        if(!InitializeCriticalSectionAndSpinCount(&mx_, 4000))
            InitializeCriticalSection(&mx_);
#   endif
    }

    ~detail(void)
    {
        DeleteCriticalSection(&mx_);
    }

    typedef CRITICAL_SECTION handle_t;
    handle_t&       operator*(void)       { return mx_; }
    const handle_t& operator*(void) const { return mx_; }

public:
    bool trylock(void)
    {
        return (!!TryEnterCriticalSection(&mx_));
    }

    void lock(void)
    {
        EnterCriticalSection(&mx_);
    }

    void unlock(void)
    {
        LeaveCriticalSection(&mx_);
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
