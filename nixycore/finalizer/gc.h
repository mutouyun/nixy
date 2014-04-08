/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/finalizer/scope_guard.h"

#include "nixycore/memory/default_alloc.h"
#include "nixycore/pattern/trackable.h"
#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"
#include "nixycore/container/container.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_gc
{
    /*
        tag for locate the gc scope
    */

    struct tag
    {
        pvoid gc_;              // pointer of gc scope

        tag(pvoid gc)
            : gc_(gc)
        {}
    };

    /*
        the resource information
    */

    struct res : tag
    {
        scope_guard sp_;        // use scope_guard to recover resource
        nx::list<tag*> links_;  // linked handles

        res(pvoid gc, const nx::functor<void()>& destructor)
            : tag(gc), sp_(destructor)
        {}

        static res* alloc(pvoid gc, const nx::functor<void()>& destructor)
        {
            return nx::alloc<res>(gc, nx::ref(destructor));
        }

        void free(void)
        {
            nx::free(this);
        }
    };

    /*
        the handle information
    */

    struct inf : tag
    {
        res* res_;              // linked res

        inf(pvoid gc)
            : tag(gc), res_(nx::nulptr)
        {}

        static inf* alloc(pvoid gc)
        {
            return nx::alloc<inf>(gc);
        }

        void free(void)
        {
            nx::free(this);
        }
    };

    //////////////////////////////////////////////////////////////////////////

    /*
        Management handlers to facilitate indexing
    */

    class manager
    {
    public:
        typedef nx::map<pvoid, inf*> map_t;
        typedef map_t::iterator iter_t;

    private:
        map_t handle_map_;

    public:
        template <typename T>
        iter_t regist(T& hd, pvoid gc)
        {
            nx_auto(ret, handle_map_.insert(map_t::value_type(static_cast<pvoid>(&hd), nx::nulptr)));
            if (ret.second)
            {
                nx_verify(ret.first->second = inf::alloc(gc));
            }
            return ret.first;
        }

        void unregist(iter_t ite)
        {
            nx_assert(ite->second);
            ite->second->free();
            handle_map_.erase(ite);
        }

        template <typename T>
        iter_t find(T& hd)
        {
            return handle_map_.find(static_cast<pvoid>(&hd));
        }
    };

    /*
        Use TLSSingleton
    */

    template <typename T>
    struct GCSingleton : TLSSingleton<T, NX_DEFAULT_ALLOC> {};
    typedef GCSingleton<manager> manager_single_t;

    //////////////////////////////////////////////////////////////////////////

    /*
        Store handles in gc scope
    */

    class handle_storage
    {
        typedef nx::map<inf*, manager::iter_t> index_map_t;

        index_map_t index_map_; // for index the iterators

    public:
        ~handle_storage(void)
        {
            nx_foreach(& pr, index_map_)
                manager_single_t::instance().unregist(pr.second);
        }

        template <typename T>
        inf* regist(T& hd, pvoid gc)
        {
            manager::iter_t ite = manager_single_t::instance().regist(hd, gc);
            if (ite->second->gc_ != gc) return ite->second;
            index_map_.insert(index_map_t::value_type(ite->second, ite));
            return ite->second;
        }

        bool insert(manager::iter_t ite)
        {
            index_map_t::value_type val(ite->second, ite);
            return index_map_.insert(val).second;
        }

        manager::iter_t erase(inf* tp)
        {
            nx_auto(ret, index_map_.find(tp));
            nx_assert(ret != index_map_.end());
            manager::iter_t ite = ret->second;
            index_map_.erase(ret);
            return ite;
        }
    };

    /*
        Store resources in gc scope
    */

    class resour_storage
    {
        typedef nx::list<res*> dest_list_t;
        typedef nx::map<res*, dest_list_t::iterator> index_map_t;

        dest_list_t dest_list_; // storage res* for destroy the resource
        index_map_t index_map_; // for index the res iterators

    public:
        ~resour_storage(void)
        {
            // collecting garbage
            nx_foreach(d, dest_list_) d->free();
        }

        res* regist(pvoid gc, const nx::functor<void()>& destructor)
        {
            dest_list_.push_front(res::alloc(gc, destructor)); // push_front for reversed destruction
            index_map_[dest_list_.front()] = dest_list_.begin();
            return dest_list_.front();
        }

        bool insert(res* tp)
        {
            nx_assert(tp);
            index_map_t::value_type val(tp, dest_list_t::iterator());
            nx_auto(ret, index_map_.insert(val));
            if (ret.second)
            {
                dest_list_.push_front(tp); // push_front for reversed destruction
                ret.first->second = dest_list_.begin();
            }
            return ret.second;
        }

        void erase(res* tp)
        {
            nx_auto(ret, index_map_.find(tp));
            nx_assert(ret != index_map_.end());
            dest_list_.erase(ret->second);
            index_map_.erase(ret);
        }
    };

    //////////////////////////////////////////////////////////////////////////

    /*
        counter for calculate the gc id
    */

    class counter : public trackable<counter, GCSingleton>
    {
        typedef trackable<counter, GCSingleton> base_t;

        int id_;

        int calculate_id(void)
        {
            counter* next = base_t::track()->next_;
            if (next) return next->id_ + 1;
            return 0;
        }

    public:
        counter(void)
            : id_(calculate_id())
        {}

        int id(void) const { return id_; }
    };

    /*
        Connect the handles and resources
    */

    class connecter : public counter
    {
        handle_storage handles_;
        resour_storage resours_;

    public:
        static connecter* get_gc(pvoid gc) { nx_assert(gc); return static_cast<connecter*>(gc); }
        template <typename T>
        static connecter* get_gc(T tp)     { nx_assert(tp); return get_gc(tp->gc_); }

        /////////////////////////////////////

        template <typename T>
        static inf* regist(T& hd, pvoid gc)
        { return get_gc(gc)->handles_.regist(hd, gc); }

        static res* regist(pvoid gc, const nx::functor<void()>& destructor)
        { return get_gc(gc)->resours_.regist(gc, destructor); }

        /////////////////////////////////////

        template <typename T>
        static manager::iter_t erase(T tp)      { return get_gc(tp)->handles_.erase(tp); }
        static res*            erase(res* tp)   { get_gc(tp)->resours_.erase(tp); return tp; }

        static bool insert(manager::iter_t ite) { return get_gc(ite->second)->handles_.insert(ite); }
        static bool insert(res* tp)             { return get_gc(tp)->resours_.insert(tp); }

        /////////////////////////////////////

        template <typename T>
        static void set_gc(T tp, pvoid gc)
        {
            if (get_gc(tp) == gc) return;
            nx_auto(ite, erase(tp));
            tp->gc_ = gc;
            insert(ite);
        }

        template <typename T>
        static pvoid check_gc(T tp, pvoid gc)
        {
            connecter* p1 = get_gc(tp);
            connecter* p2 = get_gc(gc);
            return (p1->id() < p2->id()) ? p1 : p2;
        }

        static void refresh_gc(inf* tp, pvoid gc)
        {
            refresh_gc(tp->res_, gc);
            set_gc(tp, check_gc(tp, gc));
        }

        static void refresh_gc(res* tp, pvoid gc)
        {
            nx_foreach(lk, tp->links_)
            {
                refresh_gc(static_cast<inf*>(lk), gc);
            }
            set_gc(tp, check_gc(tp, gc));
        }

        /////////////////////////////////////

        static void link_to(inf* tp1, inf* tp2)     { tp1->res_ = tp2->res_; }
        static void link_to(inf* tp1, res* tp2)     { tp1->res_ = tp2; }
        static void link_to(inf* tp1, nx::nulptr_t) { tp1->res_ = nx::nulptr; }

    private:
        nx::map<pvoid, nx::list<tag*> > res_link_tags_;

    public:
        static void remember_parent(inf* tp, pvoid parent_res)
        {
            if (!parent_res) return;
            get_gc(tp)->res_link_tags_[parent_res].push_back(tp);
        }

        template <typename T>
        static typename enable_if<is_pointer<T>::value // only pointer need link_handles
        >::type_t link_handles(const T& r, res* tp)
        {
            nx_auto(& res_link_tags, get_gc(tp)->res_link_tags_);
            nx_auto(ret, res_link_tags.find(horrible_cast<pvoid>(r)));
            if (ret == res_link_tags.end()) return;
            tp->links_.swap(ret->second);
            res_link_tags.erase(ret);
        }

        template <typename T>
        static typename enable_if<!is_pointer<T>::value
        >::type_t link_handles(const T& /*r*/, res* /*tp*/) {}
    };

    //////////////////////////////////////////////////////////////////////////

    /*
        assigner for automatic completion of connector's work
    */

    template <typename T = nx::null_t>
    class assigner;

    template <>
    class assigner<nx::null_t>
    {
        pvoid gc_;

        template <typename U>
        const U& regist_resour(const U& r, const nx::functor<void()>& destructor)
        {
            connecter::link_handles(r, connecter::regist(gc_, destructor));
            return r;
        }

    public:
        assigner(pvoid gc)
            : gc_(gc)
        {
            nx_assert(gc);
        }

        template <typename U>
        const U& operator()(const U& r)
        {
            return regist_resour(r, make_destructor(r));
        }

        template <typename U, typename F>
        const U& operator()(const U& r, const F& dest_fr)
        {
            return regist_resour(r, make_destructor(r, dest_fr));
        }

        nx::nulptr_t operator()(nx::nulptr_t)
        {
            return nx::nulptr;
        }
    };

    template <typename T>
    class assigner
    {
        T& rh_;
        inf* rh_tp_;

        void regist_handle(pvoid gc, pvoid parent_res)
        {
            rh_tp_ = connecter::regist(rh_, gc);
            connecter::remember_parent(rh_tp_, parent_res);
        }

        template <typename U>
        res* regist_resour(const U& r, const nx::functor<void()>& destructor)
        {
            res* rs_tp = connecter::regist(connecter::track(), destructor);
            connecter::link_handles(r, rs_tp);
            return rs_tp;
        }

        template <typename U>
        T& assign_to(U& r)
        {
            if (rh_ == r) return rh_;
            nx_auto(ite, manager_single_t::instance().find(r));
            connecter::refresh_gc(ite->second, rh_tp_->gc_);
            connecter::link_to(rh_tp_, ite->second);
            return rh_ = r;
        }

        template <typename U>
        T& assign_to(const U& r, const nx::functor<void()>& destructor)
        {
            if (rh_ == r) return rh_;
            res* rs_tp = regist_resour(r, destructor);
            connecter::refresh_gc(rs_tp, rh_tp_->gc_);
            connecter::link_to(rh_tp_, rs_tp);
            return rh_ = r;
        }

        T& assign_to(nx::nulptr_t)
        {
            connecter::link_to(rh_tp_, nx::nulptr);
            return rh_ = nx::nulptr;
        }

    public:
        assigner(pvoid gc, T& rh, pvoid parent_res = nx::nulptr)
            : rh_(rh), rh_tp_(nx::nulptr)
        {
            nx_assert(gc);
            regist_handle(gc, parent_res);
        }

        template <typename U>
        T& operator()(U& r)
        {
            return assign_to(r);
        }

        template <typename U>
        T& operator()(const U& r)
        {
            return assign_to(r, make_destructor(r));
        }

        template <typename U, typename F>
        T& operator()(const U& r, F dest_fr)
        {
            return assign_to(r, make_destructor(r, dest_fr));
        }

        T& operator()(nx::nulptr_t)
        {
            return assign_to(nx::nulptr);
        }

        template <typename U> T& operator=(const U& r) { return operator()(r); }
        template <typename U> T& operator=      (U& r) { return operator()(r); }
    };
}

//////////////////////////////////////////////////////////////////////////

/*
    Execute gc when the enclosing scope exits
*/

class gc_scope : private_gc::connecter
{
    pvoid gc_;
    pvoid parent_res_;

public:
#if defined(NX_CC_MSVC)
#   pragma warning(push)            // vs2005 need this
#   pragma warning(disable: 4355)   // 'this' : used in base member initializer list
#endif
    gc_scope(void)
        : gc_(this), parent_res_(nx::nulptr)
    {}
#if defined(NX_CC_MSVC)
#   pragma warning(pop)
#endif

    gc_scope(pvoid parent_res)
        : gc_(track()->next_), parent_res_(parent_res)
    {}

    using private_gc::connecter::track;

public:
    template <typename U>
    const U& operator()(const U& r)
    {
        return private_gc::assigner<>(gc_)(r);
    }

    template <typename U, typename F>
    const U& operator()(const U& r, F dest_fr)
    {
        return private_gc::assigner<>(gc_)(r, dest_fr);
    }

    template <typename T>
    private_gc::assigner<T> operator()(T& rh)
    {
        return private_gc::assigner<T>(gc_, rh, parent_res_);
    }
};

//////////////////////////////////////////////////////////////////////////

/*
    -->
    void func(void)
    {
        nx_gc_scope();
        ...
        int* pd = nx::nulptr;
        gc(pd) = nx::alloc<int>(2);
        ...
    }
*/

#define nx_gc_scope() nx::gc_scope gc

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
