/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

/*
    split
*/

static bool split_check(const_iterator i, const_iterator* t)                    // default check
{
    if (t) (*t) = i + 1;
    return (*i == L' ' || *i == L'\n' || *i == L'\t' || *i == L'\r');
}

static bool split_check(const_iterator i, const_iterator* t, value_type sep)    // check with separator
{
    if (t) (*t) = i + 1;
    return (*i == sep);
}

static bool split_check(const_iterator i, const_iterator* t, const string& sep) // check with string
{
    if (t) (*t) = i + sep.length();
    return (string(i, i + sep.length()) == sep);
}

static rvalue<vector<string>, true> split(const string& src,
                                          const functor<bool(const_iterator, const_iterator*)>& do_check,
                                          size_type limit = npos)
{
    vector<string> v;
    const_iterator i = src.begin(), last = i;
    for (; i != src.end(); ++i)
    {
        const_iterator t;
        if (!do_check(i, &t)) continue;
        if (t != src.end() && do_check(t, 0)) continue;
        v.push_back(string(last, i));
        last = t;
        if (v.size() >= limit - 1)
        {
            v.push_back(string(last, src.end()));
            return v;
        }
    }
    if (last != i)
        v.push_back(string(last, i));
    return v;
}

static rvalue<vector<string>, true> split(const string& src, size_type limit = npos)
{
    return split(src, static_cast<bool(*)(const_iterator, const_iterator*)>(&string::split_check), limit);
}

static rvalue<vector<string>, true> split(const string& src, value_type sep, size_type limit = npos)
{
    return split(src, 
                 bind(static_cast<bool(*)(const_iterator, const_iterator*, value_type)>(&string::split_check),
                      nx::_1, nx::_2, sep), 
                 limit);
}

static rvalue<vector<string>, true> split(const string& src, const string& sep, size_type limit = npos)
{
    return split(src, 
                 bind(static_cast<bool(*)(const_iterator, const_iterator*, const string&)>(&string::split_check),
                      nx::_1, nx::_2, ref(sep)), 
                 limit);
}

/*
    number transform
*/

template <typename T>
T to_number(void) const
{
    T retval = 0; long dotval = 0;
    bool neg = false;
    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (i == begin())
        {
            if (*i == L'-')
            {
                neg = true;
                continue;
            }
            else
            if (*i == L'+')
            {
                continue;
            }
        }
        if (*i >= L'0' && *i <= L'9')
        {
            if (dotval > 0) dotval *= 10;
            retval *= 10;
            retval += *i - L'0';
        }
        else
        if (dotval == 0 && *i == L'.') dotval = 1;
        else break;
    }
    if (neg)
        retval *= -1;
    if (dotval > 1)
        retval /= dotval;
    return retval;
}

//////////////////////////////////////////////////////////////////////////
