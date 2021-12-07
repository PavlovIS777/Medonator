#include "Akinator.hpp"
#include "../Shared/Tools.h"
#include <stdio.h>
#include <string.h>

int akinNodeCmp (Node* curNode, Node* newNode)
{
    char* answ = nullptr;
    scanf("%m", &answ);
    if (!strcmp("no", answ))
        return 1;
    if (!strcmp("yes", answ))
        return 0;
    assert(0 && "Wrong input answ");
}

