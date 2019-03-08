#include "conv-harness.h"
#include <stdio.h>

int main(void)
{
    char* tests[5][5] = {
    {"100", "100", "1", "5", "10"},
    {"1000", "1000", "3", "7", "6"},
    {"1000", "5000", "5", "3", "7"},
    {"200", "700", "7", "9", "8"},
    {"1500", "1775", "5", "10", "5"}
    };
    freopen("output.txt", "w+", stdout);
    for (int i = 0; i < sizeof(tests[0]) / sizeof(tests[0][0]); i++)
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