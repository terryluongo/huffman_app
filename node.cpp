#include "node.h"
#include <QDataStream>

Node::Node(int weight, int letter)
    : weight(weight), letter(letter), left(nullptr), right(nullptr)
{}

int Node::getWeight() {
    return this->weight;
}

void Node::setWeight(int weight) {
    this->weight = weight;
}

int Node::getLetter() {
    return this->letter;
}

void Node::setLetter(int letter) {
    this->letter = letter;
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

// Overload for serializing (writing) the Person object
QDataStream &operator<<(QDataStream &out, Node &node) {
    out << node.getWeight() << node.getLetter() << *node.getLeft() << *node.getRight();
    // Write each member
    return out;
}

// Overload for deserializing (reading) the Person object
QDataStream &operator>>(QDataStream &in, Node &node) {
    int weight;
    int letter;
    Node left(0,0);
    Node right(0,0);

    in >> weight >> letter >> left >> right;

    node.setLeft(&left);
    node.setRight(&right);
    node.setWeight(weight);
    node.setLetter(letter);


    return in;
}

