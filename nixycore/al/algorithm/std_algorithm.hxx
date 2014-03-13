/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
#if defined(NX_CC_MSVC)
//////////////////////////////////////////////////////////////////////////

using std::swap;
using std::copy;
using std::fill;
using std::fill_n;
using std::sort;
using std::equal;
using std::find;

//////////////////////////////////////////////////////////////////////////
#elif defined(NX_CC_GNUC)
//////////////////////////////////////////////////////////////////////////

template <typename T>
inline void swap(T& x, T& y)
{
    std::swap(x, y);
}

template <typename Iter_, typename IterR_>
inline IterR_ copy(Iter_ first, Iter_ last, IterR_ r)
{
    return std::copy(first, last, r);
}

template <typename Iter_, typename V>
inline void fill(Iter_ first, Iter_ last, const V& v)
{
    std::fill(first, last, v);
}

template <typename T, typename S, typename V>
inline T fill_n(T first, S n, const V& v)
{
    return std::fill_n(first, n, v);
}

template <typename Iter_>
inline void sort(Iter_ first, Iter_ last)
{
    std::sort(first, last);
}

template <typename Iter_, typename Comp_>
inline void sort(Iter_ first, Iter_ last, Comp_ func)
{
    std::sort(first, last, func);
}

template <typename Iter1_, typename Iter2_>
inline bool equal(Iter1_ first1, Iter1_ last1, Iter2_ first2)
{
    return std::equal(first1, last1, first2);
}

template <typename Iter1_, typename Iter2_, typename Pred_>
inline bool equal(Iter1_ first1, Iter1_ last1, Iter2_ first2, Pred_ func)
{
    return std::equal(first1, last1, first2, func);
}

template <typename Iter_, typename V>
inline Iter_ find(Iter_ first, Iter_ last, const V& v)
{
    return std::find(first, last, v);
}

//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////

/*
    using std::lexicographical_compare for compare
*/

template <typename Iter1_, typename Iter2_>
inline bool compare(Iter1_ first1, Iter1_ last1, Iter2_ first2, Iter2_ last2)
{
    return std::lexicographical_compare(first1, last1, first2, last2);
}

template <typename Iter1_, typename Iter2_, typename Comp_>
inline bool compare(Iter1_ first1, Iter1_ last1, Iter2_ first2, Iter2_ last2, Comp_ func)
{
    return std::lexicographical_compare(first1, last1, first2, last2, func);
}

//////////////////////////////////////////////////////////////////////////
