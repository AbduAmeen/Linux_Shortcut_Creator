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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
signals:
    void NewMessage(QString message);
    void CreateMessageNode(QString nickname, QString message);
private slots:
    void CreateFriendNode(QString nickname);
    void on_SubmitButton_clicked();
    void CreateMessageNode(QString message);
    void on_UserLineEdit_returnPressed();
private:
    Ui::MainWindow *ui;
    QString m_nickname;
    Network::Client m_client;
};

#endif // MAINWINDOW_H
