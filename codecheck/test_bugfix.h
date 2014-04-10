#pragma once

#include "nixycore/bugfix/bugfix.h"

//////////////////////////////////////////////////////////////////////////

void testAssert(void) nx_noexcept
{
    TEST_CASE();

    bool b = true;
    int i = 1, j = 2;
    char what[] = "What's up";

    try
    {
        nx_assert(b = false)(i)(j)(what).msg("blabla").except(b);
        strout << b << endl;
    }
    catch(const nx::assert_context&)
    {
        strout << "catched assert_context!" << endl;
    }
    catch(bool b)
    {
        strout << "catched bool: " << b << endl;
    }
    catch(...)
    {
        strout << "What?!" << endl;
    }
}

//////////////////////////////////////////////////////////////////////////

void testTrace(void)
{
    TEST_CASE();

    nx_trace("I %x Sego")("love") << nx::endl;
    nx_trace()("Sego ") << 233 << nx::endl<2>;
}

//////////////////////////////////////////////////////////////////////////

void testBugFix(void)
{
    TEST_FUNCTION();

    testAssert();
    testTrace();
}
