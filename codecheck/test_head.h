#pragma once

//////////////////////////////////////////////////////////////////////////

#ifndef CODECHECK_CUSTOM_STROUT

#include <iostream>

using std::cout;
using std::endl;

#define strout cout

#endif/*CODECHECK_CUSTOM_STROUT*/

//////////////////////////////////////////////////////////////////////////

#define TEST_FUNCTION(...) \
    strout << endl \
           << "==================================" << endl \
           << "\t" __VA_ARGS__ << __FUNCTION__ << endl \
           << "==================================" << endl

#define TEST_CASE(...) \
    strout << endl \
           << "----------------------------------" << endl \
           << "\t" __VA_ARGS__ << __FUNCTION__ << " ->: " << endl \
           << "----------------------------------" << endl

//////////////////////////////////////////////////////////////////////////

#include "test_preprocessor.h"
#include "test_bugfix.h"
#include "test_typemanip.h"
#include "test_algorithm.h"
#include "test_utility.h"
#include "test_time.h"
#include "test_random.h"
#include "test_delegate.h"
#include "test_thread.h"
#include "test_memory.h"
#include "test_finalizer.h"
#include "test_string.h"
#include "test_stream.h"

//////////////////////////////////////////////////////////////////////////

void testAll(void)
{
    //testPreprocessor();
    testBugFix();
    //testTypeManip();
    //testAlgorithm();
    //testUtility();
    //testTime();
    //testRandom();
    //testDelegate();
    //testFinalizer();
    //testThread();
//    testMemory();
    //testString();
    //testStream();

    TEST_FUNCTION(<< "Finished ");
}
