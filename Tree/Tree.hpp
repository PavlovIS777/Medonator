#ifndef TREE_H
#define TREE_H

typedef char* Data;
#define complexData 1

#if complexData == 1
#define freeData(data) free(data)
#else
#define freeData(data) 
#endif

struct Tree
{
    Node* head = nullptr;
    size_t size = 0;
}


struct Node
{
    Node* left;
    Node* right;
    Data data;
};

void treeCtor(Node** rootNode, Data data);

void treeDtor(Node* rootNode);

void addChild(Node* curNode, Node* newNode, int nodeCmp(Node* curNode, Node* newNode));



#endif