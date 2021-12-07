#include "Tree.hpp"
#include "../Shared/Tools.h"
#include <assert.h>
#include <stdlib.h>



void treeCtor(Node** rootNode, Data data)
{
    *rootNode = (Node*)safeCalloc(1, sizeof(Node));
    (*rootNode)->data = data;
}

void treeDtor(Node* rootNode)
{
    if (rootNode->left)
        treeDtor(rootNode->left);
    if (rootNode->right)
        treeDtor(rootNode->right);
    freeData(rootNode->data);
    free(rootNode);
}

void addChild(Node* curNode, Node* newNode, int nodeCmp(Node* curNode, Node* newNode))
{
    if (nodeCmp(curNode, newNode))
        curNode->left  = newNode;
    else
        curNode->right = newNode;
}