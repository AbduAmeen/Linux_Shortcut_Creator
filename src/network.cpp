#include <QTimerEvent>
#include <QCborValue>
#include <QHostInfo>

#include "network.h"
#include "mainwindow.h"

namespace  {
quint16 BroadcastPort = 2837;
QList<QHostAddress> ipaddresses;
QList<QHostAddress> broadcastaddresses;

void UpdateAddresses() {
    for (auto p : QNetworkInterface::allInterfaces()) {
        for (auto q : p.addressEntries()) {
            if (q.broadcast() != QHostAddress::Null && q.ip() != QHostAddress::LocalHost) {
                broadcastaddresses.append(q.broadcast());
                ipaddresses.append(q.ip());
            }
        }
    }
}

QList<QHostAddress> GetBroadcastAddresses() {
    return broadcastaddresses;
}

QList<QHostAddress> GetIpAddresses() {
    return ipaddresses;
}

bool IsLocalHostAddress(QHostAddress address) {
    for(auto p : GetIpAddresses()){
        if (address.isEqual(p))
            return true;
    }
    return false;
}
}

//Server class
namespace Network {
Server::Server(QObject* parent) : QTcpServer(parent) {
    listen(QHostAddress::Any);
}

void Server::incomingConnection(qintptr handle) {
    auto socket = new Connection(this);
    socket->setSocketDescriptor(handle);
    emit PendingConnection(socket);
}

}//end server

//Client class
namespace Network {
Client::Client(std::shared_ptr<Logger::Logger> logger, QObject* parent) : QObject (parent), m_logger(logger) {
    auto ptr = qobject_cast<MainWindow*>(parent);
    m_nickname = "abdu";
    m_broadcasttimer.setInterval(1000);
    m_broadcasttimer.start();
    m_broadcastsocket.bind(QHostAddress::Any, BroadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    UpdateAddresses();

    connect(&m_server, SIGNAL(PendingConnection(Connection*)), SLOT(ProcessConnection(Connection*)));
    connect(&m_broadcasttimer, SIGNAL(timeout()), SLOT(Broadcast()));
    connect(&m_broadcastsocket, SIGNAL(readyRead()), SLOT(ReadBroadcast()));
    connect(ptr, SIGNAL(NewMessage(QString)), this, SLOT(SendMessage(QString)));
}

void Client::SendMessage(QString message) {
    for (auto p : m_usersconnected)
        p->write(message.toUtf8());
}

void Client::ProcessConnection(Connection* socket) {
    connect(socket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(socket, SIGNAL(Ready()), this, SLOT(Ready()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
}

void Client::ReadMessage() {
    auto a = qobject_cast<Connection*>(sender());
    QString str;
    str.fromUtf8(a->readAll());
    emit IncomingMessage(str);
}

void Client::Disconnected() {

}

void Client::Error(QAbstractSocket::SocketError error) {
    auto a = qobject_cast<Connection*>(sender());
    m_usersconnected.remove(a->peerAddress());
}

void Client::Broadcast() {
    QByteArray array;
    QCborStreamWriter writer(&array);
    writer.startArray(1);
    writer.append(m_server.serverPort());
    writer.endArray();

    for (auto i : GetBroadcastAddresses()){
        if (m_broadcastsocket.writeDatagram(array, i, BroadcastPort) == -1)
            UpdateAddresses();
    }
}

void Client::ReadBroadcast() {
    while (m_broadcastsocket.hasPendingDatagrams()) {
        quint16 serverport = 0;
        QHostAddress senderip;
        QByteArray buffer;
        buffer.resize(m_broadcastsocket.pendingDatagramSize());
        QCborStreamReader reader(buffer);

        if (m_broadcastsocket.readDatagram(buffer.data(), buffer.size(), &senderip, &serverport) != -1){
            if (!reader.isContainer())
                continue;

            reader.enterContainer();

            if (!reader.isInteger())
                continue;
            serverport = reader.toInteger();
        }
        else {
            continue;
        }

        if (IsLocalHostAddress(senderip) && serverport == m_server.serverPort())
            continue;

        if (HasConnection(senderip, serverport))
            continue;
        auto p = new Connection(this);
        p->connectToHost(senderip, serverport);
        ProcessConnection(p);
    }
}

bool Client::HasConnection(QHostAddress address, int port) {
    if (port == -1)
        return m_usersconnected.contains(address);

    if (m_usersconnected.size() != 0){
        for(auto p : m_usersconnected) {
            if (p->peerAddress().isEqual(address))
                return true;
        }
    }
    else return false;

//    if (!m_usersconnected.contains(address))
//        return false;

    const QList<Connection *> connections = m_usersconnected.values(address);
    for (auto g : connections) {
        if (g->peerPort() == port)
            return true;
    }

    return false;
}

void Client::Ready() {
    auto socket = qobject_cast<Connection*>(sender());
    m_usersconnected.insert(socket->peerAddress(), socket);

    emit UserJoined(QString("abdu: %1").arg(socket->peerPort()));
}

Client::~Client() {
}

}//end client

namespace Network {
Connection::Connection (QObject* parent) : QTcpSocket(parent), m_writer(this) {
    connect(this, SIGNAL(connected()), SLOT(ReadyForUse()));
}

void Connection::ReadyForUse() {
    emit Ready();
}
}
