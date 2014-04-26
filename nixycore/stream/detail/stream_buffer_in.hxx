/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

void make_finish(void)
{
    index_ = fmt_.length();         // make finish
    ref_index_ = ref_->length();    // make read complete
}

void check_matching(wchar c, wchar r)
{
    if (c == r)
        ref_index_ += 1;
    else
        make_finish();
}

void put_buffer_and_wait(wchar r)
{
    string tmp;
    bool end_state = false;
    do
    {
        if (string::default_check(r))
        {
            if (end_state) break;
        }
        else
        {
            tmp.push_back(r);
            end_state = true;
        }
        ref_index_ += 1;
        if (is_read_complete()) break;
        r = ref_->at(ref_index_);
    } while (true);
    buf_.swap(tmp);
}

void do_assign_in(void)
{
    nx_assert(ref_);
    while (!is_finished())
    {
        wchar r = ref_->at(ref_index_);
        wchar c = fmt_[index_++];
        if (c == '%')
        {
            c = fmt_[index_++];
            if (c == '%')
                check_matching(c, r);
            else
                return put_buffer_and_wait(r);
        }
        else
            check_matching(c, r);
    }
}

template <typename V>
void swscanf_buf(V& val) const
{
    nx::swscanf(buf_, printf_format<V, wchar>::val(), val);
}

void parse_in(bool& val) const
{
    val = (buf_ == L"true");
}

template <typename V>
typename enable_if<is_character<V>::value
>::type_t parse_in(V& val) const
{
    if (buf_.empty()) return;
    val = buf_[0];
}

template <typename V>
typename enable_if<!is_same<V, bool>::value && 
                   !is_character<V>::value && 
                    is_numeric<V>::value
>::type_t parse_in(V& val) const
{
    swscanf_buf(val);
}

void parse_in(char* val) const
{
    strcpy(val, buf_.to_local().c_str());
}

void parse_in(wchar* val) const
{
    wcscpy(val, buf_.c_str());
}

void parse_in(pvoid& val) const
{
    swscanf_buf(val);
}

template <typename V>
typename enable_if<!is_character<V>::value && /* <MSVC 2005> */
                    is_class<V>::value
>::type_t parse_in(V& val) const
{
    buf_ >> val;
}

//////////////////////////////////////////////////////////////////////////
