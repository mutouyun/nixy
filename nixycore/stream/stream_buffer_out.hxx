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
void swprintf_buf(V val, size_t buf_count)
{
    size_t eof_index = buf_.length();
    buf_.resize(eof_index + buf_count);
#   include "nixycore/al/stream/swprintf.hxx"
    int n = NX_SWPRINTF_(const_cast<wchar*>(buf_.data()) + eof_index, buf_count, printf_format<V, wchar>::val(), val);
    if (n < 0) return;
    buf_.resize(eof_index + n);
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
                    is_integral<V>::value
>::type_t parse_out(V val)
{
    swprintf_buf(val, sizeof(val)* 3);
}

#if (NX_CC_MSVC == 1400)
void parse_out(float val)
{
    swprintf_buf(val, sizeof(val)* 6);
}

void parse_out(double val)
{
    swprintf_buf(val, sizeof(val)* 6);
}

void parse_out(ldouble val)
{
    swprintf_buf(val, sizeof(val)* 6);
}
#else
template <typename V>
typename enable_if<is_float<V>::value
>::type_t parse_out(V val)
{
    swprintf_buf(val, sizeof(val)* 6);
}
#endif

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
    swprintf_buf(val, sizeof(val)* 2 + 2); // 0xFFFFFFFF
}

template <typename V>
typename enable_if<is_class<V>::value
>::type_t parse_out(const V& val)
{
    buf_ << val;
}

//////////////////////////////////////////////////////////////////////////
