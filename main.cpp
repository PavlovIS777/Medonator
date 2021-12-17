#include "./Shared/Tools.h"
#include "./Akinator/Akinator.hpp"
#include <stdio.h>
#include <string.h>
int main(void)
{
    Akinator* akinator = initAkinator();
    doAkinator(akinator);
    for (int i = 0; i < akinator->qCount; ++i)
    {
        printf("q: %d", i);
        for (int j = 0; j < akinator->questions[i].answCnt; ++j)
        {
            printf("id: %d ", akinator->questions[i].answId[j]);
        }
        printf("\n");
    }
    return 0;
}