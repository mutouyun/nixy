#pragma once

#include "nixycore/string/string.h"

//////////////////////////////////////////////////////////////////////////

void testConverter(void)
{
    TEST_CASE();

    //printf("Hello world\n");
    //printf("你好世界\n");
    //printf("こんにちわ、世界\n");

    const char* c = "こんにちわ、世界";
    size_t n = nx::transform::utf(c, (long*)0);
    //strout << n << " " << (int)(unsigned char)*c << endl;

    //wchar_t* s = new wchar_t[n];
    //size_t m = nx::transform::utf(c, s);
    //strout << n << " " << m << endl;

    wstring s;
    size_t n; nx::wchar w;
    while (!!(n = nx::transform::utf(c, w)))
    {
        s.push_back(w);
        c += n;
    }
    FILE* fp = fopen("test_converter.txt", "wb");
    fwrite(s.c_str(), sizeof(nx::wchar), s.length(), fp);
    fclose(fp);
}

//////////////////////////////////////////////////////////////////////////

void testString(void)
{
    TEST_FUNCTION();

    testConverter();
}
