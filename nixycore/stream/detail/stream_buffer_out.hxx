/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

void do_assign_out(void)
{
    while (!is_finished())
    {
        wchar c = fmt_[index_++];
        if (c == '%')
        {
            c = fmt_[index_++];
            if (c == '%')
                buf_.push_back(c);
            else
                return; // and just wait
        }
        else
            buf_.push_back(c);
    }
}

template <typename V>
void swprintf_buf(const V& val)
{
    nx::swprintf(buf_, printf_format<V, wchar>::val(), val);
}

void parse_out(bool val)
{
    static const wchar PARSE_T_[] = L"true";
    static const wchar PARSE_F_[] = L"false";
    if (val)
        buf_.append(PARSE_T_, nx_countof(PARSE_T_) - 1);
    else
        buf_.append(PARSE_F_, nx_countof(PARSE_F_) - 1);
}

template <typename V>
typename enable_if<is_character<V>::value
>::type_t parse_out(V val)
{
    buf_.push_back(val);
}

template <typename V>
typename enable_if<!is_sametype<V, bool>::value && 
                   !is_character<V>::value && 
                    is_numeric<V>::value
>::type_t parse_out(V val)
{
    swprintf_buf(val);
}

void parse_out(const char* val)
{
    buf_ += val;
}

void parse_out(const wchar* val)
{
    buf_ += val;
}

void parse_out(pvoid val)
{
    swprintf_buf(val);
}

template <typename V>
typename enable_if<!is_character<V>::value && /* vs2005 need this */
                    is_class<V>::value
>::type_t parse_out(const V& val)
{
    buf_ << val;
}

//////////////////////////////////////////////////////////////////////////
