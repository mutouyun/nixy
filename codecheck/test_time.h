#pragma once

#include "nixycore/time/time.h"
#include "nixycore/thread/thread_ops.h"

#include <iomanip>

//////////////////////////////////////////////////////////////////////////

#ifndef STOPWATCH_CHECKER
#define STOPWATCH_CHECKER(op) \
    strout << "stopwatch ->: " << #op << endl; \
    sw.op(); \
    for(int i = 0; i < 5; ++i) \
    { \
        nx::thread_ops::sleep(100); \
        strout << "stopwatch ->: " << sw.value() << endl; \
    }
#endif

void testStopwatch(void)
{
    TEST_CASE();

    strout << std::setiosflags(std::ios_base::showpoint);
    nx::stopwatch<> sw;
    STOPWATCH_CHECKER(start)
    STOPWATCH_CHECKER(pause)
    STOPWATCH_CHECKER(start)
    STOPWATCH_CHECKER(stop)
    STOPWATCH_CHECKER(start)
    STOPWATCH_CHECKER(start)
    strout << std::resetiosflags(std::ios_base::showpoint);
}

#undef STOPWATCH_CHECKER
#undef STOPWATCH_SLEEP

//////////////////////////////////////////////////////////////////////////

void testTime(void)
{
    TEST_FUNCTION();

    testStopwatch();
}
