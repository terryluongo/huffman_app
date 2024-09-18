#include "mainwindow.h"

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

    QTableWidget *table = new QTableWidget(center);
    mainLayout->addWidget(table);
    table->setRowCount(10);
    table->setColumnCount(5);


}

MainWindow::~MainWindow() {



}

QString MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName();
    qDebug() << filename;
    return filename;
}

void MainWindow::encodeData() {

}

void MainWindow::decodeData() {

}
