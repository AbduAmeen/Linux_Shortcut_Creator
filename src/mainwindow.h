#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "network.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(std::shared_ptr<Logger::Logger> logger, QWidget *parent = nullptr);
    ~MainWindow() override;
signals:
    void NewMessage(QString message);
private slots:
    void RemoveFriendNode(QString nickname);
    void CreateFriendNode(QString nickname);
    void NewMessageNode(QString message);
    void on_SubmitButton_clicked();
    void CreateMessageNode(QString nickname, QString message);
    void on_UserLineEdit_returnPressed();
private:
    Ui::MainWindow *ui;
    QString m_nickname;
    std::unique_ptr<Network::Client> m_client;
    std::shared_ptr<Logger::Logger> m_logger;
};

#endif // MAINWINDOW_H
