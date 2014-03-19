#pragma once

#include "nixycore/string/string.h"

//////////////////////////////////////////////////////////////////////////

namespace test_string
{
    const char* c = "Hello world 你好世界 こんにちわ、世界";
}

//////////////////////////////////////////////////////////////////////////

void testTransform(void)
{
    TEST_CASE();

    using namespace test_string;

    size_t n = nx::transform::local_to_utf(c);
    wchar_t* s = new wchar_t[n];
    n = nx::transform::local_to_utf(c, s, n);
    FILE* fp = fopen("test_transform_s.txt", "wb");
    fwrite(s, sizeof(wchar_t), n - 1, fp);
    fclose(fp);
    strout << "local_to_utf size: " << n << endl;

    n = nx::transform::utf_to_local(s);
    char* z = new char[n];
    n = nx::transform::utf_to_local(s, z, n);
    fp = fopen("test_transform_z.txt", "wb");
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

void testStringDetail(void)
{
    TEST_CASE();

    using namespace test_string;

    nx::string str('c');

    str = c;
    str = str + "\r\ncstring: 123123\r\n"
              + L"wstring: 一二三一二三\r\n"
              + 'c'
              + "\r\n";
    str += L'夏';

    FILE* fp = fopen("test_string.txt", "wb");
    fwrite(str.c_str(), sizeof(wchar_t), str.length(), fp);
    fclose(fp);

    nx::vector<nx::string> arr = str.split();
    nx_foreach(s, arr)
        strout << s.to_local().c_str() << endl;
}

//////////////////////////////////////////////////////////////////////////

void testString(void)
{
    TEST_FUNCTION();

    //testTransform();
    testStringDetail();
}
