#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include "tree.h"
#include "node.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QByteArray data;
    QTableWidget *table;
    QVector<int> *frequencies;
private slots:
    void openFile();
    void encodeData();
    void decodeData();

private:
    QVector<QString> encodeHuffman(QVector<int> frequencies);
    Tree buildTree(QVector<int> frequencies);
    QMap<int, QString>* buildEncodingDict(Tree tree);
    void traverseTree(Node *node, QString path, QMap<int, QString> *encoding_dict);
    QByteArray convertBinary(QByteArray data, QMap<int, QString> *code);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
