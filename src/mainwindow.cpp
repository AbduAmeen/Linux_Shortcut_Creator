#include <QListWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagenode.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_nickname = m_client.GetNickname();
    ui->setupUi(this);
    connect(&m_client, SIGNAL(IncomingMessage(QString, QString)), this, SLOT(CreateMessageNode(QString, QString)));
    connect(&m_client, SIGNAL(NewFriend(QString)), this, SLOT(CreateFriendNode(QString)));
    connect(this, SIGNAL(NewMessage(QString)), this, SLOT(CreateMessageNode(QString)));
    connect(this, SIGNAL(NewMessage(QString)), &m_client, SLOT(SendMessage(QString)));
    ui->MessagesListWidget->setItemDelegate(new MessageNode(ui->MessagesListWidget));
    CreateFriendNode(m_nickname);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateMessageNode(QString message) {

    ui->MessagesListWidget->addItem(new QListWidgetItem(message));

}

void MainWindow::CreateMessageNode(QString nickname, QString message) {
    ui->MessagesListWidget->addItem(new QListWidgetItem(message));
}

void MainWindow::CreateFriendNode(QString nickname) {
    ui->FriendsListWidget->addItem(new QListWidgetItem(nickname));
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
