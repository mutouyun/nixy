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

template <typename IterT, typename IterR>
inline IterR copy(IterT first, IterT last, IterR r)
{
    return std::copy(first, last, r);
}

template <typename IterT, typename V>
inline void fill(IterT first, IterT last, const V& v)
{
    std::fill(first, last, v);
}

template <typename T, typename S, typename V>
inline T fill_n(T first, S n, const V& v)
{
    return std::fill_n(first, n, v);
}

template <typename IterT>
inline void sort(IterT first, IterT last)
{
    std::sort(first, last);
}

template <typename IterT, typename CompT>
inline void sort(IterT first, IterT last, CompT func)
{
    std::sort(first, last, func);
}

template <typename IterT1, typename IterT2>
inline bool equal(IterT1 first1, IterT1 last1, IterT2 first2)
{
    return std::equal(first1, last1, first2);
}

template <typename IterT1, typename IterT2, typename PredT>
inline bool equal(IterT1 first1, IterT1 last1, IterT2 first2, PredT func)
{
    return std::equal(first1, last1, first2, func);
}

template <typename IterT, typename V>
inline IterT find(IterT first, IterT last, const V& v)
{
    return std::find(first, last, v);
}

//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////

/*
    using std::lexicographical_compare for compare
*/

template <typename IterT1, typename IterT2>
inline bool compare(IterT1 first1, IterT1 last1, IterT2 first2, IterT2 last2)
{
    return std::lexicographical_compare(first1, last1, first2, last2);
}

template <typename IterT1, typename IterT2, typename CompT>
inline bool compare(IterT1 first1, IterT1 last1, IterT2 first2, IterT2 last2, CompT func)
{
    return std::lexicographical_compare(first1, last1, first2, last2, func);
}

//////////////////////////////////////////////////////////////////////////
