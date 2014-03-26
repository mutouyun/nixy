#pragma once

#include "nixycore/random/random.h"
#include "nixycore/utility/countof.h"

//////////////////////////////////////////////////////////////////////////

void testRandMT19937(void)
{
    TEST_CASE();

    nx::random<> rdm(0, 10000);
    int save[10];
    int save_index = 0, save_counter[nx_countof(save)] = {0};
    char snm[6];

    FILE* fp = fopen("test_rand_mt19937.txt", "wb");
    for (int i = 0; i < 1000000; ++i)
    {
        int x = rdm.roll<int>();
        int n = 0;
        for (; n < save_index; ++n)
        {
            if (save[n] == x)
            {
                save_counter[n] += 1;
                break;
            }
        }
        if (n >= save_index && save_index < (int)nx_countof(save))
        {
            save_counter[save_index] += 1;
            save[save_index++] = x;
        }

        memset(snm, 0, sizeof(snm));
        itoa(x, snm, 10);
        for (unsigned int m = 0; (m < nx_countof(snm)) && snm[m]; ++m)
            fwrite(snm + m, sizeof(char), 1, fp);
        fwrite("\r\n", sizeof(char), 2, fp);
    }
    fclose(fp);

    for (unsigned int n = 0; n < nx_countof(save); ++n)
        strout << save[n] << "\t->: " << save_counter[n] << endl;
}

//////////////////////////////////////////////////////////////////////////

void testRandom(void)
{
    TEST_FUNCTION();

    testRandMT19937();
}
