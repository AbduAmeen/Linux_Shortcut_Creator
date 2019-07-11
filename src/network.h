#ifndef NETWORK_H
#define NETWORK_H

#include <QDateTime>
#include <QObject>
#include <QTcpServer>
#include <QByteArrayList>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QElapsedTimer>
#include <QCborStreamWriter>
#include <QCborStreamReader>
#include <QImage>
#include <QBuffer>

namespace Network {
class Connection;
class Client;
class Server;

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
    bool IsConnected(const QHostAddress &senderIp, int senderPort = -1);
signals:
    //TODO: Set all of the user handles to be by email
    void IncomingMessage(const QString &from ,const QString &message);
    void NewFriend(const QString &nick);
    void FriendDisconnected(const QString &nick);
public slots:
    void SendMessage(const QString& message);
private:
    void RemoveConnection(Connection* connection);
    //TODO: make server a seperate Application
    Server m_server;
    QString m_nickname;
    QMultiHash<QHostAddress, Connection* > m_friendmap;
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
    inline QString GetNickname() {return m_username;}
    inline State GetState() {return m_state;}
    bool SendMessage(const QString &message);

signals:
    void ReadyForUse();
    void NewMessage(const QString& from, const QString& message);
private slots:
    void ProcessReadyRead();
    void SendPing();
private:
    void HasEnoughData();
    void ProcessData();
    QCborStreamReader m_reader;
    QCborStreamWriter m_writer;
    QString m_buffer;
    State m_state;
    QString m_username;
};
} //namespace Network
#endif // NETWORK_H
