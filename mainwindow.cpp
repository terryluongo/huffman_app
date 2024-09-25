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

}

MainWindow::~MainWindow() {
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

    //TODO: HUFFMAN NEEDS AT LEAST 2 SYMBOLS HAVE TO HARD CODE IF ONE SYMBOL IN WHOLE FILE
    data = file.readAll();

    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < data.length(); ++iPos) {
        ++(*frequencies)[(unsigned char) data[iPos]];
    }



    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem();
        index->setData(Qt::DisplayRole, iRow);
        table->setItem(iRow, 0, index);

        QTableWidgetItem *count = new QTableWidgetItem();
        count->setData(Qt::DisplayRole, (*frequencies)[iRow]);
        table->setItem(iRow, 1, count);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

        //TODO: ENCODING MAPPING BREAKS WHEN WE LOAD MORE THAN ONE FILE
        table->sortByColumn(1, Qt::DescendingOrder);
    }
}


void MainWindow::encodeData() {

    Tree tree = buildTree(*frequencies);
    QMap<int, QString> *code = buildEncodingDict(tree);



    table->sortByColumn(0, Qt::AscendingOrder);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString(code->value(iRow)));
        table->setItem(iRow, 3, index);
    }

    table->sortByColumn(1, Qt::DescendingOrder);

    convertBinary(data, code);







    QString outName = QFileDialog::getSaveFileName(this, "Save file"); //maybe filter "huffman (*.huf");
    if (outName.isEmpty()) return;

    QFile outFile(outName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(outName));
        return;
    }

    QDataStream out(&outFile);


    // if it is all Qt objects we can just do this
    out << *code;

\

}

void MainWindow::decodeData() {
    QMap<int, QString> code;


    QString inName = QFileDialog::getOpenFileName();

    QFile inFile(inName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "File broken.  Not gonna run.";
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\""), inName);
        return;
    }
    QDataStream in(&inFile);
    in >> code;



}

QByteArray MainWindow::convertBinary(QByteArray data, QMap<int, QString> *code) {
    QString stringStream("");
    for (int i = 0; i < data.size(); i++)
        stringStream += code->value(data[i]);
    qDebug() << stringStream.size();
//10110010

    QByteArray out((stringStream.size() + 7) / 8, Qt::Uninitialized);
    // do string by byte
    // premake QBytearray with the right size-> (stringlength + 7) / 8
    // +7 is to round up

    // index and set QBytearray to that byte string base 2

    // do all full bytes
    for (int i = 0; i < stringStream.size() / 8; i++) {
        out[i] = stringStream.sliced(i * 8, 8).toInt(nullptr, 2);
    }

    // do remainder bytes

    // have to do .write() and .read() for the unserialized bytes




    return data;
}

Tree MainWindow::buildTree(QVector<int> frequencies) {

    // from chatgpt
    // Custom comparison functor to prioritize smaller weights (min-heap)
    struct CompareTree {
        bool operator()(const Tree& n1, const Tree& n2) {
            return n1.root->getWeight() > n2.root->getWeight();
        }
    };

    std::priority_queue<Tree, std::vector<Tree>, CompareTree> queue;

    for (int i = 0; i < frequencies.size(); i++) {
        if (frequencies[i] > 0)
            queue.push(Tree(frequencies[i], i));
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
    if (node->getLetter() != -1) {
        encoding_dict->insert(node->getLetter(), path);
        return;
    }
    traverseTree(node->getLeft(), path + "0", encoding_dict);
    traverseTree(node->getRight(), path + "1", encoding_dict);
}


// traverse tree to figure out binary representation of each string, return in QMap
QMap<int, QString>* MainWindow::buildEncodingDict(Tree tree) {
    QMap<int, QString> *encoding_dict = new QMap<int, QString>();
    traverseTree(tree.root, "", encoding_dict);
    return encoding_dict;
}




