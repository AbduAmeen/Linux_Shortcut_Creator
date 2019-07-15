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
    void IncomingMessage(QString nickname, QString message);
private slots:
    void Broadcast();
    void ReadBroadcast();
    void Disconnected();
    void ReadMessage(QString nickname, QString message);
    bool HasConnection(QHostAddress address, int port = -1);
    void ProcessConnection(Connection* socket);
    void Error(QAbstractSocket::SocketError error);
    void Ready();
private:
    void RemoveConnection(Connection* connection);
    QUdpSocket m_broadcastsocket;
    QTimer m_broadcasttimer;
    QMultiHash<QHostAddress, Connection*> m_usersconnected;
    QString m_nickname;
    std::shared_ptr<Logger::Logger> m_logger;
    Server m_server;
};

class Connection : public QTcpSocket {
    Q_OBJECT
public:
    enum State{Unconnected, ReadyToRead};
    enum DataType {Text, Ping, Pong, Undefined};
    Connection(QObject* parent = nullptr);
    Connection(qintptr descriptor, QObject* parent = nullptr);
    ~Connection();
    void WriteMessage(QString string);
    inline void SetNickname(QString nickname) {m_nickname = nickname;}
    inline QString GetNickname() {return m_nickname;}
signals:
    void Ready();
    void IncomingMessage(QString nickname, QString message);
private slots:
    void ReadyForUse();
    void ReadMessage();
    void ProcessMessage();
    void SendPing();
    void SendPong();
private:
    void ProcessGreeting();
    State m_state;
    DataType m_type;
    bool m_greetingsent;
    QString m_nickname;
    QTimer m_pingtimer;
    int m_transfertimerid;
    QElapsedTimer m_pongtimer;
    QCborStreamWriter m_writer;
    QCborStreamReader m_reader;
    QString m_buffer;
};

} //namespace Network
#endif // NETWORK_H
