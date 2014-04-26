/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/pattern/singleton.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, template <typename> class SingleT = Singleton>
class trackable
{
public:
    T * prev_, * next_;

private:
    static T *(& get_head_ptr(void))
    {
        return SingleT<T*>::instance(nx::nulptr);
    }

    void init(void)
    {
        T*(& head) = get_head_ptr();
        // check and push self to list
        if (head)
        {
            this->next_ = head;
            head->prev_ = static_cast<T*>(this);
        }
        head = static_cast<T*>(this);
    }

    void dest(void)
    {
        T*(& head) = get_head_ptr();
        // check and pop self from list
        if (!head) return;
        if (this->prev_)
        {
            this->prev_->next_ = this->next_;
            if (this->next_)
                this->next_->prev_ = this->prev_;
        }
        else
        {
            nx_assert(this == head);
            head = head->next_;
            if (head) head->prev_ = nx::nulptr;
        }
    }

public:
    trackable(void)
        : prev_(nx::nulptr)
        , next_(nx::nulptr) { init(); }

    trackable(const trackable&)
        : prev_(nx::nulptr)
        , next_(nx::nulptr) { init(); }

    ~trackable(void) { dest(); }

    trackable& operator=(const trackable&) { return (*this); }

public:
    static T* track(void) { return get_head_ptr(); }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
