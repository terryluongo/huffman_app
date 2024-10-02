#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <QString>

class Huffman
{
public:
    Huffman();
    static QVector<QString> encodeHuffman(QVector<int> frequencies);
    static QByteArray binaryToHuffman(QByteArray data, QVector<QString> code, int &remLength);
    static QMap<QByteArray, QPair<QByteArray, QByteArray>> calculateQTree(QVector<int> frequencies, QByteArray &parent);
    static void calculateEncodings(QMap<QByteArray, QPair<QByteArray, QByteArray>> &map,
                            QByteArray &parent, QVector<QString> &encodings, QByteArray current, QString path);
    static QByteArray huffmanToBinary(QByteArray raw, QMap<QByteArray, QPair<QByteArray, QByteArray>> map,
                                int remLength, QByteArray &parent, QByteArray current);
};

#endif // HUFFMAN_H
