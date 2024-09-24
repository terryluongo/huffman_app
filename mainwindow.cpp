#include "mainwindow.h"
#include <typeinfo>
#include "tree.h"
#include <queue>

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

    QVector<int> frequencies(256, 0);
    for (int iPos = 0; iPos < data.length(); ++iPos) {
        ++frequencies[(unsigned char) data[iPos]];
    }

    qDebug() << frequencies;



    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString::number(iRow));
        table->setItem(iRow, 0, index);

        QTableWidgetItem *count = new QTableWidgetItem(QString::number(frequencies[iRow]));
        table->setItem(iRow, 1, count);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

        //TODO: have to sort not lexographically
        table->sortByColumn(1, Qt::DescendingOrder);

      //  std::priority_queue<Tre queue;

    }
    /* huffman encoding notes
   sd
QMap<QByteArray, QPair<QByteArray, QByteArray>>
parent maps to its two children


     */
}

QVector<QString> MainWindow::encodeHuffman(QVector<int> frequencies) {

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
        qDebug() << frequencies[i];

    }
}

void MainWindow::encodeData() {

}

void MainWindow::decodeData() {

}
