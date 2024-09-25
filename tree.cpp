#include "tree.h"
#include <QtCore/qdebug.h>
#include <sstream>
Tree::Tree(int weight, int letter) {
    this->root = new Node(weight, letter);
}

Tree::Tree(Node* left, Node* right) {
    this->root = new Node(left->getWeight() + right->getWeight(), -1);
    this->root->setLeft(left);
    this->root->setRight(right);
}

std::string Tree::buildString(Node* node) {
    if (node == nullptr) {
        return "";
    }

    std::stringstream ss;

    // Pre-order traversal: current node, then left and right subtrees
    ss << "(Letter: " << node->getLetter() << ", Weight: " << node->getWeight() << ")";

    if (node->getLeft() || node->getRight()) {
        ss << " [Left: " << buildString(node->getLeft())
        << ", Right: " << buildString(node->getRight()) << "]";
    }

    return ss.str();
}

// Overload the operator<< for qDebug
QDebug operator<<(QDebug dbg, Tree& tree) {
    dbg.nospace() << "Tree: " << QString::fromStdString(tree.buildString(tree.root));
    return dbg.space();
}
