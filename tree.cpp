#include "tree.h"

Tree::Tree(int weight, int letter) {
    this->root = new Node(weight, letter);
}

Tree::Tree(Node* left, Node* right) {
    this->root = new Node(left->getWeight() + right->getWeight(), -1);
    this->root->setLeft(left);
    this->root->setRight(right);
}
