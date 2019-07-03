#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagenode.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MessageNode messagenode;
}

MainWindow::~MainWindow()
{
    delete ui;
}
