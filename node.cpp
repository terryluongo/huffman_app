#include "node.h"

Node::Node(int weight, int letter) {
    this->weight = weight;
    this->letter = letter;
}

int Node::getWeight() {
    return this->weight;
}

void Node::setWeight(int weight) {
    this->weight = weight;
}

int Node::getLetter() {
    return this->letter;
}

Node* Node::getLeft() {
    return this->left;
}

void Node::setLeft(Node* left) {
    this->left = left;
}

Node* Node::getRight() {
    return this->right;
}

void Node::setRight(Node* right) {
    this->right = right;
}
