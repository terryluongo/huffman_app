#include "huffman.h"
#include <QString>
#include <QMap>

Huffman::Huffman() {}

QByteArray Huffman::binaryToHuffman(QByteArray data, QVector<QString> code, int &remLength) {
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

QByteArray Huffman::huffmanToBinary(QByteArray raw, QMap<QByteArray, QPair<QByteArray, QByteArray>> map,
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

    if (remLength == 0) return bytes; // don't have to deal with remainder

    unsigned char byte = raw[raw.size() - 1];
    //i apologize for this for loop initialization; dealing with remainder
    for (int mask = 128 >> (8 - remLength); mask > 0; mask = mask >> 1) {
        if (current.size() == 1) {
            bytes.append(current);
            current = parent;
        }
        current = byte & mask ? map[current].second : map[current].first;
    }

    return bytes;
}

QMap<QByteArray, QPair<QByteArray, QByteArray>> Huffman::calculateQTree(QVector<int> frequencies, QByteArray &parent) {

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

void Huffman::calculateEncodings(QMap<QByteArray, QPair<QByteArray, QByteArray>> &map,
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

