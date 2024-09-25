#include "mainwindow.h"
#include <typeinfo>
#include "tree.h"
#include <queue>
#include <QMap>
#include "node.h"
#include <QString>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *center = new QWidget();

    setCentralWidget(center);

    QVBoxLayout *mainLayout = new QVBoxLayout(center);

    QHBoxLayout *menubar = new QHBoxLayout(center);

    mainLayout->addLayout(menubar);

    QPushButton *openFileButton = new QPushButton(center);
    openFileButton->setText("Load");
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::openFile);
    menubar->addWidget(openFileButton);

    QPushButton *encodeDataButton = new QPushButton(center);
    encodeDataButton->setText("Encode");
    connect(encodeDataButton, &QPushButton::clicked, this, &MainWindow::encodeData);
    menubar->addWidget(encodeDataButton);

    QPushButton *decodeDataButton = new QPushButton(center);
    decodeDataButton->setText("Decode");
    connect(decodeDataButton, &QPushButton::clicked, this, &MainWindow::decodeData);
    menubar->addWidget(decodeDataButton);

    table = new QTableWidget(center);
    mainLayout->addWidget(table);
    table->setRowCount(256);
    table->setColumnCount(4);



    //QByteArray *decodedFile = new QByteArray();

}

MainWindow::~MainWindow() {


// we need for table
// code, decoded character, # occurences, encoded character

}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName();
    qDebug() << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "File broken.  Not gonna run.";
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\""), filename);
        return;
    }

    data = file.readAll();

    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < data.length(); ++iPos) {
        ++(*frequencies)[(unsigned char) data[iPos]];
    }
    //qDebug() << frequencies;



    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString::number(iRow));
        table->setItem(iRow, 0, index);

        QTableWidgetItem *count = new QTableWidgetItem(QString::number((*frequencies)[iRow]));
        table->setItem(iRow, 1, count);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

        //TODO: have to sort not lexographically
        //table->sortByColumn(1, Qt::DescendingOrder);


    }


}

Tree MainWindow::buildTree(QVector<int> frequencies) {

    // from chat gpt
    // Custom comparison functor to prioritize smaller weights (min-heap)
    struct CompareTree {
        bool operator()(const Tree& n1, const Tree& n2) {
            // Return true if n1 has a lower priority than n2
            // This will make the priority queue a min-heap (smaller weights have higher priority)
            return n1.root->getWeight() > n2.root->getWeight();
        }
    };

    std::priority_queue<Tree, std::vector<Tree>, CompareTree> queue;
    for (int i = 0; i < frequencies.size(); i++) {
        queue.push(Tree(frequencies[i], i));
        //qDebug() << frequencies[i];
    }
    while (queue.size() > 1) {
        Tree first = queue.top();
        queue.pop();
        Tree second = queue.top();
        queue.pop();
        Tree combined = Tree(first.root, second.root);
        queue.push(combined);
    }

    return queue.top();


}

// helper method for buildEncodingDict method
// DFS, keeps track of the path traveled
// once we reach leaf node, add letter to HashMap with corresponding path
void MainWindow::traverseTree(Node *node, QString path, QMap<int, QString>* encoding_dict) {
    //if (node == nullptr)
      //  return;
    if (node->getLetter() != -1) {

        encoding_dict->insert(node->getLetter(), path);
        qDebug() << "Inserted:" << node->getLetter() << "Path:" << path;  // Debug output

        return;
    }
    //if (node->getLeft() != nullptr)
        traverseTree(node->getLeft(), path + "0", encoding_dict);
    //if (node->getRight() != nullptr)
        traverseTree(node->getRight(), path + "1", encoding_dict);
}

// issue is when we uncomment this
// either w/ this or traverseTree
// traverse tree to figure out binary representation of each string, return in HashMap
QMap<int, QString>* MainWindow::buildEncodingDict(Tree tree) {
    QMap<int, QString> *encoding_dict = new QMap<int, QString>();
    traverseTree(tree.root, "", encoding_dict);
    return encoding_dict;
}




void MainWindow::encodeData() {
    //QVector<int> test_frequencies = {4, 1, 2, 3, 8, 20, 6};
    Tree tree = buildTree(*frequencies);
    QMap<int, QString> *code = buildEncodingDict(tree);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString(code->take(iRow)));
        table->setItem(iRow, 3, index);




    }

}

void MainWindow::decodeData() {

}
