#pragma once

#include "nixycore/string/string.h"
#include "nixycore/stream/stream.h"

//////////////////////////////////////////////////////////////////////////

void testStreamDetail(void)
{
    TEST_CASE();

    nx::string text;
    {
        nx::format(&text, L"pi: %x%x\ntest bool: %x and %x") << 3.1415 << "926" << true << false;
    }
    strout << text.to_local().c_str() << endl << endl;
    {
        nx::io(&text) << 3.1415 << L"926" << true << false;
    }
    strout << text.to_local().c_str() << endl << endl;
    {
        text = L"I think is true && -3.1415926 && I-love-Sego";
        bool is_what = false;
        float pi = 0;
        nx::string tmp;
        nx::format(&text, L"I think is %x && %x && %x") >> is_what >> pi >> tmp;
        strout << is_what << " " << pi << " " << tmp.to_local().c_str() << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void testStream(void)
{
    TEST_FUNCTION();

    testStreamDetail();
}
