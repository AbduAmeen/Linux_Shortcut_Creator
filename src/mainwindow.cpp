#include <QListWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagenode.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,SIGNAL(NewMessage(QString)), this, SLOT(CreateMessageNode(QString)));
    ui->MessagesListWidget->setItemDelegate(new MessageNode(ui->MessagesListWidget));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateMessageNode(QString message) {

    ui->MessagesListWidget->addItem(new QListWidgetItem(message));
}
void MainWindow::on_SubmitButton_clicked()
{
    QString message = ui->UserLineEdit->text();

    if (message.isEmpty()) {return;}
    emit NewMessage(message);
}

void MainWindow::on_UserLineEdit_returnPressed()
{
    on_SubmitButton_clicked();
}
