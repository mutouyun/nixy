//#include "nixycore/time/time.h"
#include "nixycore/memory/memory.h"
//#include "nixycore/thread/thread.h"

#include <iostream>

typedef nx::skip_array<int*, 100, 1, NX_DEFAULT_ALLOC> skip_array;

void test(const skip_array& arr)
{
    skip_array::const_iterator ite;
    for(ite = arr.begin(); ite != arr.end(); ++ite)
        std::cout << *(arr[ite.index()]) << std::endl;
}

void dummy(void)
{
    //int* pp = &(nx::singleton<int>());
/*
    skip_array arr;
    arr[67] = new int(100);
    skip_array::iterator ite;
    for(ite = arr.begin(); ite != arr.end(); ++ite)
        std::cout << *(*ite) << std::endl;
    test(arr);*/
}
