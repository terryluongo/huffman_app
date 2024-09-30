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


    QByteArray root;
    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents = calculateQTree(*frequencies, root);

    //for (auto iPC = parents.begin(); iPC != parents.end(); ++iPC)
    //    qDebug() << iPC.key() << " -> " << iPC.value().first << iPC.value().second;


    QVector<QString> encodings(256,0);
    calculateEncodings(parents, root, encodings, root, "");


    table->sortByColumn(0, Qt::AscendingOrder);

    for (int iRow = 0; iRow < 256; ++iRow) {
        QTableWidgetItem *index = new QTableWidgetItem(QString(encodings[iRow]));
        table->setItem(iRow, 3, index);
    }

    table->sortByColumn(1, Qt::DescendingOrder);

    int remLength = 0;

    QByteArray binaryData = convertBinary(data, encodings, remLength);

    QString outName = QFileDialog::getSaveFileName(this, "Save file"); //maybe filter "huffman (*.huf");
    if (outName.isEmpty()) return;

    QFile outFile(outName);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::information(this, "Error", QString("Can't write to file \"%1\"").arg(outName));
        return;
    }

    QDataStream out(&outFile);

    out << parents << root << remLength;

    // skip to the good stuff with constData
    out.writeBytes(binaryData.constData(), binaryData.size());


}

void MainWindow::decodeData() {


    QString inName = QFileDialog::getOpenFileName();

    QFile inFile(inName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() << "File broken.  Not gonna run.";
        QMessageBox::information(this, "Error", QString("Can't open file \"%1\""), inName);
        return;
    }
    QDataStream in(&inFile);

    QMap<QByteArray, QPair<QByteArray, QByteArray>> parents;
    QByteArray root;
    int remLength;

    in >> parents >> root >> remLength;

    char* rawBytes; uint length;
    in.readBytes(rawBytes, length);
    QByteArray bytes(rawBytes, length);
    delete[] rawBytes;
    // the bytes aren't being encoded and decoded for some reason: length 0 when decoding

    QString reconstructed = reconstructString(bytes, parents, remLength, root, root);
}


QByteArray MainWindow::convertBinary(QByteArray data, QVector<QString> code, int &remLength) {
    QString stringStream("");

    for (int i = 0; i < data.size(); i++) {
        unsigned char byteValue = static_cast<unsigned char>(data[i]);
        unsigned int casted = static_cast<unsigned int>(byteValue);
        stringStream += code[casted];
    }
    qDebug() << stringStream.size();

    QByteArray out((stringStream.size() + 7) / 8, Qt::Uninitialized);

    for (int i = 0; i < stringStream.size() / 8; i++) {
        out[i] = stringStream.sliced(i * 8, 8).toInt(nullptr, 2);
    }


    // convert remainder to int
    int remainder = stringStream.chopped(stringStream.size() % 8).toInt(nullptr, 2);
    // if string not evenly divisble set last one to remainder
    if (stringStream.size() % 8)
        out[out.size() - 1] = remainder;

    remLength = stringStream.size() % 8;
    return out;

    // do remainder bytes

    // have to do .write() and .read() for the unserialized bytes

}

QString MainWindow::reconstructString(QByteArray raw, QMap<QByteArray, QPair<QByteArray, QByteArray>> map,
                                      int remLength, QByteArray &parent, QByteArray current) {

    QString text("");
    qDebug() << "hi";
    int length = raw.size();
    if (remLength != 0) length--;
    qDebug() << length;
    for (int i = 0; i < 3; i++) {
        qDebug() << i;
        qDebug() << "hello";
        unsigned char byte = raw[i];
        qDebug() << "bye";

        for (int mask = 128; mask > 0; mask = mask >> 1) {
            if (current.size() == 1) {
                text += QString(current);
                current = parent;
            }
            current = byte & mask ? map[current].second : map[current].first;
        }
    }
    qDebug() << text.first(200);
    return text;
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


    /*
    for (auto iPC = parentChildren.begin(); iPC != parentChildren.end(); ++iPC)
        qDebug() << iPC.key() << " -> " << iPC.value().first << iPC.value().second;
    */
}

void MainWindow:: calculateEncodings(QMap<QByteArray, QPair<QByteArray, QByteArray>> &map,
                                    QByteArray &parent, QVector<QString> &encodings, QByteArray current, QString path) {
    if (current.size() == 1) {
        // was having issues with certain values so doing this; got online
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




