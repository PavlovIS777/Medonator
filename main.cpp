#include "./Tree/Tree.hpp"
#include "./Shared/Tools.h"
#include "./Akinator/Akinator.hpp"
#include <stdio.h>
#include <string.h>
int main(void)
{
    Node* root = nullptr;
    Data str = (Data)safeCalloc(10, sizeof(char));
    strcpy(str, "kekmem");
    treeCtor(&root, str);
    printf("Hello world!");
    treeDtor(root);
    return 0;
}