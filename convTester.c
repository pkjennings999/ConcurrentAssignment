#include "conv-harness.h"
#include <stdio.h>

int main(void)
{
    char* tests[6][5] = {
    {"16", "16", "1", "32", "32"}, //Small everything
    {"256", "256", "1", "32", "128"}, //Small ord, chan
    {"16", "16", "7", "256", "32"}, //Small kern, wid, hei
    {"16", "16", "1", "32", "64"},
    {"32", "32", "1", "64", "32"},
    {"16", "16", "7", "32", "32"}
    };
    /*
    * Use -S to generate assembly code
    * Download perf for performance info. perf stat -e
    */

    freopen("FINALboth.txt", "w+", stdout);
    for (int i = 0; i < 6; i++)
    {
        printf("%s, %s, %s %s, %s\n",  tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]);
        char* argArray[6] = {"conv-harness",
        tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]};
        for (int j = 0; j < 10; j++)
        {
            mainCall(6, argArray);
        }
        printf("\n\n");
    }
}
