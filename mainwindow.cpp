#include "mainwindow.h"
#include <typeinfo>
#include <queue>
#include <QMap>
#include <QString>
#include "huffman.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *center = new QWidget();
    setCentralWidget(center);

    QVBoxLayout *mainLayout = new QVBoxLayout(center);
    QHBoxLayout *menubar = new QHBoxLayout(center);
    mainLayout->addLayout(menubar);

    QPushButton *openFileButton = new QPushButton("Load", center);
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::openFile);
    menubar->addWidget(openFileButton);

    QPushButton *encodeDataButton = new QPushButton("Encode", center);
    connect(encodeDataButton, &QPushButton::clicked, this, &MainWindow::encodeData);
    menubar->addWidget(encodeDataButton);

    QPushButton *decodeDataButton = new QPushButton("Decode", center);
    connect(decodeDataButton, &QPushButton::clicked, this, &MainWindow::decodeData);
    menubar->addWidget(decodeDataButton);

    table = new QTableWidget(256, 4, center);
    mainLayout->addWidget(table);

}


MainWindow::~MainWindow() {
}

void MainWindow::openFile() {

    QFile *file = returnQFile(QIODevice::ReadOnly | QIODevice::Text, false);
    if (file == nullptr) return;
    data = file->readAll();

    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < data.length(); ++iPos) {
        ++(*frequencies)[(unsigned char) data[iPos]];
    }

    populateTable(*frequencies);

}

void MainWindow::encodeData() {

    QByteArray root;
    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents = Huffman::calculateQTree(*frequencies, root);

    QVector<QString> encodings(256,0);
    Huffman::calculateEncodings(parents, root, encodings, root, "");

    populateTable(QVector<int>(), encodings);

    int remLength = 0;
    QByteArray binaryData = Huffman::binaryToHuffman(data, encodings, remLength);

    QFile *outFile = returnQFile(QIODevice::WriteOnly | QIODevice::Truncate, true);
    if (outFile == nullptr) return;

    QDataStream out(outFile);
    out.setVersion(QDataStream::Qt_5_15);

    int size = binaryData.size();
    out << parents << root << remLength << size;
    out.writeRawData(binaryData.constData(), size);

    int oldSize = QFileInfo(QFile(filename)).size() / 1000;
    int newSize = QFileInfo(*outFile).size() / 1000;
    QMessageBox::information(this, "Ok",
                             QString("Original File Size: %1kb\nNew File Size: %2kb").arg(oldSize).arg(newSize));

}

void MainWindow::decodeData() {

    QFile *inFile = returnQFile(QIODevice::ReadOnly, false);
    if (inFile == nullptr) return;

    QDataStream in(inFile);
    in.setVersion(QDataStream::Qt_5_15);


    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents;
    QByteArray root; int remLength; int size;
    in >> parents >> root >> remLength >> size;

    QByteArray bytes;
    bytes.resize(size);
    in.readRawData(bytes.data(), size);

    QByteArray reconstructed = Huffman::huffmanToBinary(bytes, parents, remLength, root, root);
    QVector<QString> encodings(256,0);
    Huffman::calculateEncodings(parents, root, encodings, root, "");


    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < reconstructed.length(); ++iPos)
        ++(*frequencies)[(unsigned char) reconstructed[iPos]];

    populateTable(*frequencies, encodings);
    qDebug() << "done!";

    QFile *saveFile = returnQFile(QIODevice::WriteOnly | QIODevice::Truncate, true);
    if (saveFile == nullptr) return;

    QDataStream save(saveFile);
    save.setVersion(QDataStream::Qt_5_15);

    save.writeBytes(reconstructed.constData(), reconstructed.size());
}

QFile* MainWindow::returnQFile(QFile::OpenMode flags, bool saveFlag) {

    QString thisFilename = saveFlag ? QFileDialog::getSaveFileName() : QFileDialog::getOpenFileName();
    if (!saveFlag) filename = thisFilename; // need for size comparison later

    QFile *file = new QFile(thisFilename);
    if (!file->open(flags)) {
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\"").arg(thisFilename));
        return nullptr;
    }
    return file;
}


void MainWindow::populateTable(QVector<int> frequencies, QVector<QString> encodings) {

    table->sortByColumn(0, Qt::AscendingOrder);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem();
        index->setData(Qt::DisplayRole, iRow);
        table->setItem(iRow, 0, index);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

    }

    if (frequencies.size() > 0) {
        for (int iRow = 0; iRow < 256; ++iRow) {
            QTableWidgetItem *count = new QTableWidgetItem();
            count->setData(Qt::DisplayRole, frequencies[iRow]);
            table->setItem(iRow, 1, count);
        }
    }

    if (encodings.size() > 0) {
        for (int iRow = 0; iRow < 256; ++iRow) {
            QTableWidgetItem *index = new QTableWidgetItem(QString(encodings[iRow]));
            table->setItem(iRow, 3, index);
        }
    }

    table->sortByColumn(1, Qt::DescendingOrder);
}

