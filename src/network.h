#ifndef NETWORK_H
#define NETWORK_H
#include <QObject>
#include <QTcpServer>
#include <QByteArrayList>
#include <QTcpSocket>
#include <QCborStreamReader>
#include <QCborStreamReader>
#include <QImage>
#include <QThread>
#include <QBuffer>

namespace Network {
class Connection;
class Client;
class Server;

struct MessageContainer {
    std::vector<QImage> image;
    QString text;
};

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server(QObject* parent = nullptr);
signals:
    void NewConnection(Connection *connection);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

class Client : public QObject {
    Q_OBJECT
    enum ConnectionStatus {Online, Offline};
public:
    Client();
    inline QString GetNickname() {return m_nickname;}
    inline QString GetEmail() {return m_email;}
signals:
    //TODO: Set all of the user handles to be by email
    void NewMessage(const QString &from ,const QString &message);
    void NewParticipant(const QString &nick);
    void ParticipantDisconnected(const QString &nick);
public slots:
    void SendMessage(const QString& message);
private:
    //TODO: make server a seperate thread
    Server server;
    QString m_nickname;
    QString m_email;
    QString m_password;

private slots:
    void NewConnection(Connection* connection);
    void ConnectionError(QAbstractSocket::SocketError error);
    void Disconnected();
    void Ready();
};

class Connection : public QTcpSocket {
    Q_OBJECT
public:
    enum DataType {Ping, Pong, Image, Text, Undefined};
    enum State {Ready, NotConnected, Working};
    Connection(QObject *parent = nullptr);
    Connection(qintptr socketdesc, QObject* parent = nullptr);
    ~Connection();
    inline QString GetName() {return m_username;}
    inline State GetState() {return m_state;}
    bool SendMessage(const MessageContainer &message);

signals:
    void ReadyForUse();
    void NewMessage(const QString& from, const MessageContainer& message);

private:
    QCborStreamReader m_cborreader;
    QCborStreamWriter m_cborwriter;
    State m_state;
    QString m_username;

private slots:

};
} //namespace Network
#endif // NETWORK_H
