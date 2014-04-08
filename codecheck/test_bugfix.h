#pragma once

#include "nixycore/bugfix/bugfix.h"

//////////////////////////////////////////////////////////////////////////

void testAssert(void)
{
    TEST_CASE();

    bool b = true;
    int i = 1, j = 2;
    char what[] = "What's up";

    try
    {
        nx_assert(b = false)(i)(j)(what).msg("~blabla~").except(b);
        strout << b << endl;
    }
    catch(nx::assert_context)
    {
        strout << "catched assert_context!" << endl;
    }
    catch(bool b)
    {
        strout << "catched bool: " << b << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void testTrace(void)
{
    TEST_CASE();

    nx_trace("I %x Sego")("love") << nx::endl;
    nx_trace()("love") << 123 << nx::endl;
}

//////////////////////////////////////////////////////////////////////////

void testBugFix(void)
{
    TEST_FUNCTION();

    testAssert();
    testTrace();
}
