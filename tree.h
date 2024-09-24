#ifndef TREE_H
#define TREE_H
#include "node.h"
class Tree
{
public:
    Tree(int weight, int character);
    Tree(Node* left, Node* right);

    Node* root;

};

#endif // TREE_H
