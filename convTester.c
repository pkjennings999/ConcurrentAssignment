#include "conv-harness.h"
#include <stdio.h>

int main(void)
{
    char* tests[6][5] = {
    {"16", "16", "5", "1024", "128"},
    {"32", "32", "3", "512", "256"},
    {"64", "64", "1", "256", "64"},
    {"128", "128", "7", "128", "128"},
    {"256", "256", "5", "64", "64"},
    {"512", "512", "7", "32", "32"}
    };

    // for (int i = 0; i < 10; i++)
    // {
    //     int sum = 0;
    //     for (int j = 0; j < 10; j++)
    //     {
    //         sum += i * j;
    //     }
    //     printf("sum of %d = %d\n", i, sum);
    // }

    // for (int i = 0; i < 10; ++i)
    // {
    //     int sum = 0;
    //     for (int j = 0; j < 10; ++j)
    //     {
    //         sum += i * j;
    //     }
    //     printf("sum of %d = %d\n", i, sum);
    // }
    /*
    * Use -S to generate assembly code
    * Download perf for performance info. perf stat -e
    */

    freopen("vectorisation.txt", "w+", stdout);
    for (int i = 0; i < 6; i++)
    {
        printf("%s, %s, %s %s, %s\n",  tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]);
        char* argArray[6] = {"conv-harness",
        tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]};
        for (int j = 0; j < 5; j++)
        {
            mainCall(6, argArray);
        }
        printf("\n\n");
    }
}