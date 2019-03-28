#include "conv-harness.h"
#include <stdio.h>

int main(void)
{
    char* tests[6][5] = {
    {"32", "32", "3", "512", "256"}, //Small 
    {"64", "64", "1", "32", "128"}, //Med
    {"16", "16", "7", "256", "32"}  //Large
    };
    /*
    * Use -S to generate assembly code
    * Download perf for performance info. perf stat -e
    */

    freopen("FINAL.txt", "w+", stdout);
    for (int i = 0; i < 1; i++)
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
