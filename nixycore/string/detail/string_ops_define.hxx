/*
    The Nixy Library
    Code covered by the MIT License

    Modified from Extended STL string (http://www.gotroot.ca/ext_string)
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2005, Keenan Tims (ktims@gotroot.ca) All rights reserved.
*/

//////////////////////////////////////////////////////////////////////////

/*
    split string to a vector
*/

static bool default_check(value_type c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

static bool split_check(const_iterator i, const_iterator* t)                    // default check
{
    if (t) (*t) = i + 1;
    return default_check(*i);
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

nx_rval(vector<string>, true) split(const functor<bool(const_iterator, const_iterator*)>& do_check,
                                    size_type limit = npos) const
{
    vector<string> v;
    const_iterator i = begin(), last = i;
    for (; i != end(); ++i)
    {
        const_iterator t;
        if (!do_check(i, &t)) continue;
        if (t != end() && do_check(t, nx::nulptr)) continue;
        v.push_back(string(last, i));
        last = t;
        if (v.size() >= limit - 1)
        {
            v.push_back(string(last, end()));
            return v;
        }
    }
    if (last != i)
        v.push_back(string(last, i));
    return nx::move(v);
}

nx_rval(vector<string>, true) split(size_type limit = npos) const
{
    return split(static_cast<bool(*)(const_iterator, const_iterator*)>(&string::split_check), limit);
}

nx_rval(vector<string>, true) split(value_type sep, size_type limit = npos) const
{
    return split(bind(static_cast<bool(*)(const_iterator, const_iterator*, value_type)>(&string::split_check),
                      nx::_1, nx::_2, nx_fval(sep)),
                 limit);
}

nx_rval(vector<string>, true) split(const string& sep, size_type limit = npos) const
{
    return split(bind(static_cast<bool(*)(const_iterator, const_iterator*, const string&)>(&string::split_check),
                      nx::_1, nx::_2, nx_fval(sep)),
                 limit);
}

nx_rval(vector<string>, true) split_to_chunks(size_type chunk_len) const
{
    vector<string> v;
    v.reserve(size() / chunk_len + 1);
    size_type count = 0;
    const_iterator i = begin(), last = i;
    for (; i != end(); ++i, ++count)
    {
        if (count == chunk_len)
        {
            count = 0;
            v.push_back(string(last, i));
            last = i;
        }
    }
    if (last != i)
        v.push_back(string(last, i));
    return nx::move(v);
}

/*
    replace
*/

string& replace(const string& needle, const string& s)
{
    size_type lastpos = 0, thispos;
    while ((thispos = find(needle, lastpos)) != npos)
    {
        base_t::replace(thispos, needle.length(), s);
        lastpos = thispos + 1;
    }
    return (*this);
}

string& replace(value_type needle, value_type c)
{
    for (iterator i = begin(); i != end(); ++i)
        if (*i == needle) *i = c;
    return (*this);
}

/*
    string multiplication
*/

nx_rval(string) operator*(size_type n)
{
    string ret;
    for (size_type i = 0; i < n; ++i)
        ret.append(*this);
    return nx::move(ret);
}

/*
    case conversion
*/

string& to_lower(void)
{
    for (iterator i = begin(); i != end(); ++i)
        if (*i >= 'A' && *i <= 'Z')
            *i += ('a' - 'A');
    return (*this);
}

string& to_upper(void)
{
    for (iterator i = begin(); i != end(); ++i)
        if (*i >= 'a' && *i <= 'z')
            *i -= ('a' - 'A');
    return (*this);
}

string& swap_case(void)
{
    for (iterator i = begin(); i != end(); ++i)
    {
        if (*i >= 'A' && *i <= 'Z')
            *i += ('a' - 'A');
        else
        if (*i >= 'a' && *i <= 'z')
            *i -= ('a' - 'A');
    }
    return (*this);
}

/*
    count the substring from this string
*/

size_type count(const string& sub) const
{
    size_type num = 0, last = 0, cur = 0;
    while ((cur = find(sub, last + 1)) != npos)
    {
        num += 1;
        last = cur;
    }
    return num;
}

/*
    judgement
*/

bool is_alphabet(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i < 'A' || (*i > 'Z' && (*i < 'a' || *i > 'z')))
            return false;
    return true;
}

bool is_allnumber(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i < '0' || *i > '9')
            return false;
    return true;
}

bool is_integral(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (i == begin() && (*i == '-' || *i == '+'))
            continue;
        if (*i < '0' || *i > '9')
            return false;
    }
    return true;
}

bool is_float(void) const
{
    if (empty()) return false;
    bool dot = false;
    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (i == begin() && (*i == '-' || *i == '+'))
            continue;
        if (!dot && *i == '.')
        {
            dot = true;
            continue;
        }
        if (*i < '0' || *i > '9')
            return false;
    }
    return dot;
}

bool is_numeric(void) const
{
    if (empty()) return false;
    bool dot = false;
    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (i == begin() && (*i == '-' || *i == '+'))
            continue;
        if (!dot && *i == '.')
        {
            dot = true;
            continue;
        }
        if (*i < '0' || *i > '9')
            return false;
    }
    return true;
}

bool is_nonsymbol(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (*i < 'A' || *i > 'Z')
            if (*i < '0' || *i > '9')
                if (*i < 'a' || *i > 'z')
                    return false;
    }
    return true;
}

bool is_lower(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i < 'a' || *i > 'z')
            return false;
    return true;
}

bool is_upper(void) const
{
    if (empty()) return false;
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i < 'A' || *i > 'Z')
            return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////
