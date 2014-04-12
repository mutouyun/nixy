/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"

#include "nixycore/pattern/singleton.h"
#include "nixycore/thread/thread_model.h"
#include "nixycore/thread/lock_guard.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"
#include "nixycore/bugfix/bugfix.h"

#include <map> // std::map

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace mem_leak
{
    /*
        Memory leak registration node
    */

    struct node_t
    {
        const char* file_;
        unsigned    line_;
        size_t      size_;

        static node_t* alloc(const char* file, unsigned line, size_t size)
        {
            node_t* np = nx::alloc<use::alloc_std, node_t>();
            nx_assert(np)(file)(line)(size);
            np->file_ = file;
            np->line_ = line;
            np->size_ = size;
            return np;
        }

        void free(void)
        {
            nx::free<use::alloc_std>(this);
        }
    };

    /*
        Memory leak registration storage map
    */

    typedef std::map<pvoid, node_t*> register_t;

    /*
        Memory leak collector
    */

    template <class ModelT>
    class collector
    {
        template <class Model_>
        friend class collector;

    protected:
        typedef ModelT thread_model_t;
        typedef typename thread_model_t::mutex_t mutex_t;

        register_t blocks_;
        mutex_t lc_;

    public:
        template <typename T>
        T* regist(T* p, const char* file, unsigned line, size_t size)
        {
            if (!p) return NULL;
            register_t::iterator it;
            {
                nx_lock_scope(lc_);
                it = blocks_.find(p);
            }
            if (it == blocks_.end())
            {
                node_t* np = node_t::alloc(file, line, size);
                nx_assert(np)(file)(line)(size);
                nx_lock_scope(lc_);
                blocks_[p] = np;
            }
            else if (it->second->size_ != size)
            {
                node_t* np = node_t::alloc(file, line, size);
                nx_assert(np)(file)(line)(size);
                it->second->free();
                it->second = np;
            }
            return p;
        }

        void unregist(pvoid p)
        {
            if (!p) return;
            register_t::iterator it;
            {
                nx_lock_scope(lc_);
                it = blocks_.find(p);
            }
            if (it != blocks_.end())
            {
                it->second->free();
                nx_lock_scope(lc_);
                blocks_.erase(it);
            }
        }

        template <class Model_>
        void merge(const collector<Model_>& other)
        {
            nx_lock_scope(lc_);
            register_t::const_iterator it = other.blocks_.begin();
            for (; it != other.blocks_.end(); ++it)
            {
                blocks_.insert(register_t::value_type(it->first, it->second));
            }
        }
    };

    /*
        Memory leak recorder, whole process has only one recorder
    */

    class recorder : public collector<NX_DEFAULT_THREAD_MODEL>
    {
    public:
        static recorder& instance(void)
        {
            static recorder& cache = nx::singleton<recorder>();
            return cache;
        }

        ~recorder(void)
        {
            report(); // When process exit, print report
        }

        void report(void) const
        {
            if (blocks_.empty())
                nx::trace(L"%xNo memory leaks!") << nx::endl<2>;
            else
            {
                nx::trace(L"%xDetect memory leaks! (%x)") << nx::endl << blocks_.size() << nx::endl<2>;
                register_t::const_iterator it = blocks_.begin();
                for (; it != blocks_.end(); ++it)
                {
                    nx_assert(it->first);
                    nx_assert(it->second);
                    nx::trace(L"\"%x\"(%x) ->: %x (%x)")
                            << it->second->file_ << it->second->line_
#               if defined(NX_OS_WIN)
                            << nx::fmt(L"0x%p", it->first)
#               else
                            << it->first
#               endif
                            << it->second->size_ << nx::endl;
                    for (size_t n = 0, i = 0; i < 2; ++i)
                    {
                        for (size_t j = 0; (j < 8) && (n < it->second->size_); ++j, ++n)
                            nx::trace(L"%x ") << nx::fmt(L"%02X", static_cast<uchar*>(it->first)[n]);
                        if (n >= it->second->size_) break;
                        nx::trace(L" ");
                    }
                    nx::trace() << nx::endl;
                }
                nx::trace(L"%xReport finish...") << nx::endl<3>;
            }
        }
    };

    /*
        Thread local recorder
    */

    class tls_recorder : public collector<use::thread_single>
    {
    public:
        static tls_recorder& instance(void)
        {
            recorder::instance(); // Make recorder run first
            return TLSSingleton<tls_recorder, use::alloc_std>::instance();
        }

        ~tls_recorder(void)
        {
            // When thread exit, merge self into the recorder
            recorder::instance().merge(*this);
        }
    };

    /*
        Memory leak catcher
    */

    class catcher
    {
        const char* file_;
        unsigned    line_;

    public:
        catcher(const char* file, unsigned line)
            : file_(file)
            , line_(line)
        {}

        /* catch alloc */

        template <class AllocT>
        pvoid alloc(size_t size)
        {
            return tls_recorder::instance().regist
                (nx::alloc<AllocT>(size), file_, line_, size);
        }

        pvoid alloc(size_t size)
        {
            return alloc<NX_DEFAULT_ALLOC>(size);
        }

        template <class AllocT, typename T>
        T* alloc(void)
        {
            return tls_recorder::instance().regist
                (nx::alloc<AllocT, T>(), file_, line_, sizeof(T));
        }

        template <typename T>
        T* alloc(void)
        {
            return alloc<NX_DEFAULT_ALLOC, T>();
        }

#   define NX_ALLOC_(n) \
        template <class AllocT, typename T, NX_PP_TYPE_1(n, typename P)> \
        T* alloc(NX_PP_TYPE_2(n, P, NX_PP_FPAR(par))) \
        { \
            return tls_recorder::instance().regist \
                (nx::alloc<AllocT, T>(NX_PP_FORWARD(n, P, par)), file_, line_, sizeof(T)); \
        } \
        template <typename T, NX_PP_TYPE_1(n, typename P)> \
        T* alloc(NX_PP_TYPE_2(n, P, NX_PP_FPAR(par))) \
        { \
            return alloc<NX_DEFAULT_ALLOC, T>(NX_PP_FORWARD(n, P, par)); \
        }
        NX_PP_MULT_MAX(NX_ALLOC_)
#   undef NX_ALLOC_

        /* catch free */

        template <class AllocT>
        void free(pvoid p, size_t size)
        {
            tls_recorder::instance().unregist(p);
            nx::free<AllocT>(p, size);
        }

        void free(pvoid p, size_t size)
        {
            free<NX_DEFAULT_ALLOC>(p, size);
        }

        template <class AllocT, typename T>
        void free(T* p)
        {
            tls_recorder::instance().unregist(p);
            nx::free<AllocT>(p);
        }

        template <typename T>
        void free(T* p)
        {
            free<NX_DEFAULT_ALLOC>(p);
        }

        /* catch realloc */

        template <class AllocT>
        pvoid realloc(pvoid p, size_t old_size, size_t new_size)
        {
            if (p && old_size)
            {
                if (new_size == 0)
                {
                    free(p, old_size);
                    return NULL;
                }
                return do_realloc(p, nx::realloc<AllocT>(p, old_size, new_size), new_size);
            }
            return alloc(new_size);
        }

        pvoid realloc(pvoid p, size_t old_size, size_t new_size)
        {
            return realloc<NX_DEFAULT_ALLOC>(p, old_size, new_size);
        }

        template <class AllocT, typename T>
        pvoid realloc(T* p, size_t size)
        {
            if (p)
            {
                if (size == 0)
                {
                    free(p);
                    return NULL;
                }
                return do_realloc(p, nx::realloc<AllocT>(p, size), sizeof(T));
            }
            return alloc(size);
        }

        template <typename T>
        pvoid realloc(T* p, size_t size)
        {
            return realloc<NX_DEFAULT_ALLOC>(p, size);
        }

    private:
        pvoid do_realloc(pvoid p, pvoid new_p, size_t new_size)
        {
            tls_recorder::instance().regist(new_p, file_, line_, new_size);
            if (new_p != p) tls_recorder::instance().unregist(p);
            return new_p;
        }
    };
}

/*
    Redefine macros
*/

#ifndef NX_ENABLE_MEMLEAK_DETECT
#ifndef NDEBUG
#   define NX_ENABLE_MEMLEAK_DETECT
#endif
#endif/*NX_ENABLE_MEMLEAK_DETECT*/

#ifdef NX_ENABLE_MEMLEAK_DETECT
#undef  nx_alloc
#undef  nx_free
#undef  nx_realloc
#define nx_alloc    nx::mem_leak::catcher(__FILE__, __LINE__).alloc
#define nx_free     nx::mem_leak::catcher(__FILE__, __LINE__).free
#define nx_realloc  nx::mem_leak::catcher(__FILE__, __LINE__).realloc
#endif/*NX_ENABLE_MEMLEAK_DETECT*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
