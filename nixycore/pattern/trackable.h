/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "pattern/singleton.h"

#include "bugfix/assert.h"

#include "general/general.h"
#include "typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, template <typename> class Single_ = Singleton>
class Trackable
{
    typedef Single_<T*> single_t;

public:
    T * prev_, * next_;

private:
    void init(void)
    {
        T*(& head) = single_t::instance();
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
        T*(& head) = single_t::instance();
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
    Trackable(void)
        : prev_(nx::nulptr)
        , next_(nx::nulptr) { init(); }

    Trackable(const Trackable&)
        : prev_(nx::nulptr)
        , next_(nx::nulptr) { init(); }

    ~Trackable() { dest(); }

    Trackable& operator=(const Trackable&) { return (*this); }

public:
    static T* track(void) { return single_t::instance(); }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
