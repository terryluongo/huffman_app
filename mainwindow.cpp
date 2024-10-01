#include "mainwindow.h"
#include <typeinfo>
#include <queue>
#include <QMap>
#include <QString>
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

    filename = QFileDialog::getOpenFileName();
    qDebug() << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "File broken.  Not gonna run.";
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\"").arg(filename));
        return;
    }

    //TODO: HUFFMAN NEEDS AT LEAST 2 SYMBOLS HAVE TO HARD CODE IF ONE SYMBOL IN WHOLE FILE
    data = file.readAll();

    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < data.length(); ++iPos) {
        ++(*frequencies)[(unsigned char) data[iPos]];
    }


    table->sortByColumn(0, Qt::AscendingOrder);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem();
        index->setData(Qt::DisplayRole, iRow);
        table->setItem(iRow, 0, index);

        QTableWidgetItem *count = new QTableWidgetItem();
        count->setData(Qt::DisplayRole, (*frequencies)[iRow]);
        table->setItem(iRow, 1, count);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

        table->sortByColumn(1, Qt::DescendingOrder);
    }
}


void MainWindow::encodeData() {

    QByteArray root;
    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents = calculateQTree(*frequencies, root);

    QVector<QString> encodings(256,0);
    calculateEncodings(parents, root, encodings, root, "");

    // have to sort so it is filled in properly
    table->sortByColumn(0, Qt::AscendingOrder);
    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString(encodings[iRow]));
        table->setItem(iRow, 3, index);
    }
    table->sortByColumn(1, Qt::DescendingOrder);

    int remLength = 0;
    QByteArray binaryData = convertBinary(data, encodings, remLength);
    qDebug() << binaryData.size();

    QString outName = QFileDialog::getSaveFileName(this, "Save file"); //maybe filter "huffman (*.huf");
    if (outName.isEmpty()) return;

    QFile outFile(outName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(outName));
        return;
    }

    QDataStream out(&outFile);
    out.setVersion(QDataStream::Qt_5_15);

    int size = binaryData.size();
    out << parents << root << remLength << size;
    out.writeRawData(binaryData.constData(), size);

    int oldSize = QFileInfo(QFile(filename)).size() / 1000;
    int newSize = QFileInfo(outFile).size() / 1000;
    QMessageBox::information(this, "Ok",
                             QString("Original File Size: %1kb\nNew File Size: %2kb").arg(oldSize).arg(newSize));

}

void MainWindow::decodeData() {


    QString inName = QFileDialog::getOpenFileName();

    QFile inFile(inName);
    if (!inFile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\"").arg(inName));
        return;
    }
    QDataStream in(&inFile);
    in.setVersion(QDataStream::Qt_5_15);


    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents;
    QByteArray root; int remLength; int size;
    in >> parents >> root >> remLength >> size;

    QByteArray bytes;
    bytes.resize(size);
    in.readRawData(bytes.data(), size);

    QByteArray reconstructed = reconstructBytes(bytes, parents, remLength, root, root);
    QVector<QString> encodings(256,0);
    calculateEncodings(parents, root, encodings, root, "");


    frequencies = new QVector<int>(256, 0);
    for (int iPos = 0; iPos < reconstructed.length(); ++iPos)
        ++(*frequencies)[(unsigned char) reconstructed[iPos]];


    table->sortByColumn(0, Qt::AscendingOrder);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem();
        index->setData(Qt::DisplayRole, iRow);
        table->setItem(iRow, 0, index);

        QTableWidgetItem *count = new QTableWidgetItem();
        count->setData(Qt::DisplayRole, (*frequencies)[iRow]);
        table->setItem(iRow, 1, count);

        QTableWidgetItem *character = new QTableWidgetItem(QString((char) iRow));
        table->setItem(iRow, 2, character);

        QTableWidgetItem *codings = new QTableWidgetItem(QString(encodings[iRow]));
        table->setItem(iRow, 3, codings);

        //TODO: ENCODING MAPPING BREAKS WHEN WE LOAD MORE THAN ONE FILE
    }
    table->sortByColumn(1, Qt::DescendingOrder);
    qDebug() << "done!";


    QString saveName = QFileDialog::getSaveFileName(this, "Save file"); //maybe filter "huffman (*.huf");
    if (saveName.isEmpty()) return;

    QFile saveFile(saveName);
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(saveName));
        return;
    }

    QDataStream save(&saveFile);
    save.setVersion(QDataStream::Qt_5_15);

    save.writeBytes(reconstructed.constData(), reconstructed.size());
}


QByteArray MainWindow::convertBinary(QByteArray data, QVector<QString> code, int &remLength) {
    QString stringStream("");

    for (int i = 0; i < data.size(); i++) {
        unsigned char byteValue = static_cast<unsigned char>(data[i]);
        unsigned int casted = static_cast<unsigned int>(byteValue);
        stringStream += code[casted];
    }

    QByteArray out((stringStream.size() + 7) / 8, Qt::Uninitialized);
    for (int i = 0; i < stringStream.size() / 8; i++) {
        out[i] = stringStream.sliced(i * 8, 8).toInt(nullptr, 2);
    }

    // convert remainder to int, if string not evenly divisble set last byte to remainder
    int remainder = stringStream.chopped(stringStream.size() % 8).toInt(nullptr, 2);
    if (stringStream.size() % 8)
        out[out.size() - 1] = remainder;

    remLength = stringStream.size() % 8; //need to return 2 things so I have this passed by reference

    return out;

}

QByteArray MainWindow::reconstructBytes(QByteArray raw, QMap<QByteArray, QPair<QByteArray, QByteArray>> map,
                                      int remLength, QByteArray &parent, QByteArray current) {
    QByteArray bytes;
    int length = raw.size();
    if (remLength != 0) length--;

    for (int i = 0; i < length; i++) {
        unsigned char byte = raw[i];
        for (int mask = 128; mask > 0; mask = mask >> 1) {
            if (current.size() == 1) {
                bytes.append(current);
                current = parent;
            }
            current = byte & mask ? map[current].second : map[current].first;
        }
    }

    if (remLength == 0) return bytes; // 't have to deal with remainder

    unsigned char byte = raw[raw.size() - 1];
    //i apologize for this for loop initialization; dealing with remainder
    for (int mask = 128 >> (8 - remLength); mask > 0; mask = mask >> 1) {
        if (current.size() == 1) {
            bytes.append(current);
            current = parent;
        }
        current = byte & mask ? map[current].second : map[current].first;
    }

    qDebug() << bytes.size();
    return bytes;
}

QMap<QByteArray, QPair<QByteArray, QByteArray>> MainWindow::calculateQTree(QVector<int> frequencies, QByteArray &parent) {

    QMultiMap<int, QByteArray> toDo;  // Maps a frequency to the QByteArray it corresponds to
    for (int code = 0; code < 256; ++code)
        if (frequencies[code] > 0)
            toDo.insert(frequencies[code], QByteArray(1, code));


    QMap<QByteArray, QPair<QByteArray, QByteArray> > parentChildren;
    while (toDo.size() > 1) {
        int freq0 = toDo.begin().key();
        QByteArray chars0 = toDo.begin().value();
        toDo.erase(toDo.begin());

        int freq1 = toDo.begin().key();
        QByteArray chars1 = toDo.begin().value();
        toDo.erase(toDo.begin());

        int parentFreq = freq0 + freq1;
        QByteArray parentChars = chars0 + chars1;
        toDo.insert(parentFreq, parentChars);

        parentChildren[parentChars] = qMakePair(chars0, chars1);
    }

    parent = toDo.begin().value();
    return parentChildren;

}

void MainWindow:: calculateEncodings(QMap<QByteArray, QPair<QByteArray, QByteArray>> &map,
                                    QByteArray &parent, QVector<QString> &encodings, QByteArray current, QString path) {
    if (current.size() == 1) {
        // was having issues with certain values and signed vs unsigned so doing this; got online
        unsigned char byteValue = static_cast<unsigned char>(current[0]);
        unsigned int casted = static_cast<unsigned int>(byteValue);
        encodings[casted] = path;
        return;
    }

    QPair<QByteArray, QByteArray> children = map.value(current);
    calculateEncodings(map, parent, encodings, children.first, path + "0");
    calculateEncodings(map, parent, encodings, children.second, path + "1");

    return;
}




