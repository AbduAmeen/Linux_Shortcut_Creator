#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMultiHash>
#include <QNetworkInterface>
#include <QCborStreamReader>
#include <QCborStreamWriter>

#include "logger.h"

namespace  {
    QList<QHostAddress> GetBroadcastAddresses();
    QList<QHostAddress> GetIpAddresses();
    void UpdateAddresses();
}

namespace Network {
class Connection;
class Client;
class Server;
class Connection;

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server(QObject* parent = nullptr);
signals:
    void PendingConnection(Connection* connection);
protected:
    void incomingConnection(qintptr handle) override;
};

class Client : public QObject {
    Q_OBJECT

public:
    Client(std::shared_ptr<Logger::Logger> logger, QObject* parent = nullptr);
    ~Client();
    inline int GetServerPort() {return m_server.serverPort();}
public slots:
    void SendMessage(QString message);
signals:
    void UserLeft(QString nickname);
    void UserJoined(QString nickname);
    void IncomingMessage(QString string);
private slots:
    void Broadcast();
    void ReadBroadcast();
    void Disconnected();
    void ReadMessage();
    bool HasConnection(QHostAddress address, int port = -1);
    void ProcessConnection(Connection* socket);
    void Error(QAbstractSocket::SocketError error);
    void Ready();
private:
    QUdpSocket m_broadcastsocket;
    QTimer m_broadcasttimer;
    QMultiHash<QHostAddress, Connection*> m_usersconnected;
    QString m_nickname;
    std::shared_ptr<Logger::Logger> m_logger;
    Server m_server;
};

class Connection : public QTcpSocket {
    Q_OBJECT
    enum DataType {Text, Ping, Pong };
public:
    Connection(QObject* parent = nullptr);
    Connection(qintptr pointer, QObject* parent = nullptr);
signals:
    void Ready();
private slots:
    void ReadyForUse();
    //WriteMessage(QString string);
private:
    QCborStreamReader m_reader;
    QCborStreamWriter m_writer;
    QString m_buffer;

};

} //namespace Network
#endif // NETWORK_H
