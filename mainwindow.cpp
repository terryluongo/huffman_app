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


}

MainWindow::~MainWindow() {



}

QString MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName();
    qDebug() << filename;
    return filename;
}
