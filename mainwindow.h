#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#include <QFileDialog>
#include <QMap>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QByteArray data;
    QTableWidget *table;
    QVector<int> *frequencies;
    QWidget *center;
private slots:
    void openFile();
    void encodeData();
    void decodeData();

private:
    QVector<QString> encodeHuffman(QVector<int> frequencies);
    QByteArray convertBinary(QByteArray data, QVector<QString> code, int &remLength);
    QMap<QByteArray, QPair<QByteArray, QByteArray>> calculateQTree(QVector<int> frequencies, QByteArray &parent);
    void calculateEncodings(QMap<QByteArray, QPair<QByteArray, QByteArray>> &map,
                            QByteArray &parent, QVector<QString> &encodings, QByteArray current, QString path);
    QByteArray reconstructBytes(QByteArray raw, QMap<QByteArray, QPair<QByteArray, QByteArray>> map,
                              int remLength, QByteArray &parent, QByteArray current);





public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
