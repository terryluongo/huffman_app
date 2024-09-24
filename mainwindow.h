#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#include <QFileDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QByteArray data;
    QTableWidget *table;
private slots:
    void openFile();
    void encodeData();
    void decodeData();

private:
    QVector<QString> encodeHuffman(QVector<int> frequencies);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
