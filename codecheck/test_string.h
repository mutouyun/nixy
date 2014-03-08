#pragma once

#include "nixycore/string/string.h"

//////////////////////////////////////////////////////////////////////////

void testTransform(void)
{
    TEST_CASE();

    const char* c = "Hello world 你好世界 こんにちわ、世界";
    size_t n = nx::transform::local_to_utf(c);
    wchar_t* s = new wchar_t[n];
    n = nx::transform::local_to_utf(c, s, n);
    FILE* fp = fopen("test_converter_s.txt", "wb");
    fwrite(s, sizeof(wchar_t), n - 1, fp);
    fclose(fp);
    strout << "local_to_utf size: " << n << endl;

    n = nx::transform::utf_to_local(s);
    char* z = new char[n];
    n = nx::transform::utf_to_local(s, z, n);
    fp = fopen("test_converter_z.txt", "wb");
    fwrite(z, sizeof(char), n - 1, fp);
    fclose(fp);
    strout << "utf_to_local size: " << n << endl;

    //wchar_t* s = new wchar_t[n];
    //size_t m = nx::transform::utf(c, s);
    //strout << n << " " << m << endl;
/*
    wstring s;
    size_t n; nx::wchar w;
    while (!!(n = nx::transform::utf(c, w)))
    {
        s.push_back(w);
        c += n;
    }*/
}

//////////////////////////////////////////////////////////////////////////

void testString(void)
{
    TEST_FUNCTION();

    testTransform();
}
