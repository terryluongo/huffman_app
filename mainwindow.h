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
    QString filename;
    void populateTable(QVector<int> frequencies = QVector<int>(), QVector<QString> encodings = QVector<QString>());
    QFile* returnQFile(QFile::OpenMode flags, bool saveFlag);



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
