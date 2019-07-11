#ifndef NETWORK_H
#define NETWORK_H

#include <QDateTime>
#include <QObject>
#include <QTcpServer>
#include <QByteArrayList>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QNetworkInterface>
#include <QElapsedTimer>
#include <QCborStreamWriter>
#include <QCborStreamReader>
#include <QBuffer>
#include <QHash>

namespace Network {
class Connection;
class Client;
class Server;
class FriendManager;

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
    FriendManager* m_friendmanager;
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
    enum DataType {Ping, Pong, Greeting, Text, Undefined};
    enum State {Ready, WaitingForGreeting, ReadingGreeting};
    Connection(QObject *parent = nullptr);
    Connection(qintptr socketdesc, QObject* parent = nullptr);
    ~Connection();
    inline QString GetNickname() {return m_nickname;}
    inline State GetState() {return m_state;}
    bool SendMessage(const QString &message);
    inline void SetGreeting(QString string) {m_greetingmessage = string;}
protected:
    void timerEvent(QTimerEvent *timerEvent) override;
signals:
    void ReadyForUse();
    void NewMessage(const QString& from, const QString& message);
private slots:
    void ProcessReadyRead();
    void SendGreeting();
    void SendPing();
private:
    void ProcessGreeting();
    void HasEnoughData();
    void ProcessData();

    int m_transfertimerid;
    DataType m_currentdatatype;
    QTimer m_timer;
    QElapsedTimer m_elapesdtimer;
    QCborStreamReader m_reader;
    QCborStreamWriter m_writer;
    QString m_buffer;
    QString m_nickname;
    QString m_greetingmessage;
    State m_state;
    bool m_isgreetingsent;
};

class FriendManager : public QObject {
    Q_OBJECT
public:
    FriendManager(Client* client);
    inline void SetServerPort(int port) {m_serverport = port;}
    inline QString GetNickname() {return m_nickname;}
    void StartBroadcasting();
    bool IsLocalHostAddress(const QHostAddress &address);
signals:
    void NewConnection(Connection* connection);
private slots:
    void SendBroadcastDatagram();
    void ReadBroadcastDatagram();
private:
    void UpdateAddresses();

    Client* m_client;
    QString m_nickname;
    QList<QHostAddress> m_broadcastaddresses;
    QList<QHostAddress> m_ipaddresses;
    QUdpSocket m_broadcastsocket;
    QTimer m_broadcasttimer;
    int m_serverport;
};
} //namespace Network
#endif // NETWORK_H
