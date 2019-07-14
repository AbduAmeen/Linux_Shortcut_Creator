#include <QListWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagenode.h"

MainWindow::MainWindow(std::shared_ptr<Logger::Logger> logger,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_logger(logger)
{
    m_client = std::make_unique<Network::Client>(m_logger,this);
    ui->setupUi(this);
    ui->MessagesListWidget->setItemDelegate(new MessageNode(ui->MessagesListWidget));
    ui->MessagesListWidget->setResizeMode(QListView::Adjust);
    m_nickname = "abdu" + QString(": %1").arg(m_client->GetServerPort());

    connect(this, SIGNAL(NewMessage(QString)), this, SLOT(NewMessageNode(QString)));
    connect(m_client.get(), SIGNAL(IncomingMessage(QString)), this, SLOT(NewMessageNode(QString)));
    connect(m_client.get(), SIGNAL(UserJoined(QString)), this, SLOT(CreateFriendNode(QString)));
    connect(m_client.get(), SIGNAL(UserLeft(QString)), this, SLOT(RemoveFriendNode(QString)));
    CreateFriendNode(m_nickname);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::NewMessageNode(QString message) {
    ui->MessagesListWidget->addItem(new QListWidgetItem(message));
}

void MainWindow::CreateMessageNode(QString nickname, QString message) {
    ui->MessagesListWidget->addItem(new QListWidgetItem(message));
}

void MainWindow::CreateFriendNode(QString nickname) {
    ui->FriendsListWidget->addItem(new QListWidgetItem(nickname));
}

void MainWindow::RemoveFriendNode(QString nickname) {
    for (int var = 0; var < ui->FriendsListWidget->count(); ++var) {
        QString text = ui->FriendsListWidget->item(var)->text();
        if (nickname == text) {
            delete ui->FriendsListWidget->item(var);
        }
    }
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
