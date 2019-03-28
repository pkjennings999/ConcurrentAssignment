#include "conv-harness.h"
#include <stdio.h>

int main(void)
{
    char* tests[6][5] = {
    {"32", "32", "3", "512", "256"}, //Small 
    {"64", "64", "1", "256", "64"}, //Med
    {"128", "128", "7", "128", "128"}  //Large
    };

    //Uncomment to print to a file
    // freopen("FINAL.txt", "w+", stdout);
    for (int i = 0; i < 3; i++)
    {
        printf("%s, %s, %s %s, %s\n",  tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]);
        char* argArray[6] = {"conv-harness",
        tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]};
        for (int j = 0; j < 2; j++)
        {
            mainCall(6, argArray);
        }
        printf("\n\n");
    }
}
