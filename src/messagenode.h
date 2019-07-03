#ifndef MESSAGENODE_H
#define MESSAGENODE_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QDateTime>

namespace Ui {
class MessageNode;
}

class MessageNode : public QWidget
{
    Q_OBJECT

public:
    explicit MessageNode(QWidget *parent = nullptr);
    ~MessageNode();
    QImage GetProfileImage();
    QString GetMessage();
    QDateTime GetDateTime();
private:
    Ui::MessageNode *ui;
    QImage m_profileimage;
    QLabel* m_imagebox;
    QLabel* m_datetimelabel;
    QString m_message;
    QDateTime m_datetime;
    QTextEdit* m_textedit;
};

#endif // MESSAGENODE_H
