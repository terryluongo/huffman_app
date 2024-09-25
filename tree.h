#ifndef TREE_H
#define TREE_H
#include "node.h"
#include <QtCore/qdebug.h>
#include <string>
class Tree
{
public:
    Tree(int weight, int character);
    Tree(Node* left, Node* right);

    Node* root;

    std::string buildString(Node* node);
    friend QDebug operator<<(QDebug dbg, Tree& tree);

};

#endif // TREE_H
