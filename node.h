#ifndef NODE_H
#define NODE_H

class Node {

public:

    Node(int weight, int letter);

    int getWeight();
    void setWeight(int weight);

    int getLetter();

    Node* getLeft();
    void setLeft(Node* left);

    Node* getRight();
    void setRight(Node* right);

private:

    int weight;
    int letter;

    Node* left;
    Node* right;
};

#endif // NODE_H
