#include <QTimerEvent>
#include <QCborValue>
#include <QHostInfo>

#include "network.h"
#include "mainwindow.h"

namespace  {
quint16 BroadcastPort = 2837;
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
    for (auto p : m_usersconnected) {
        if (!message.isEmpty())
            p->WriteMessage(message);
    }
}

void Client::ProcessConnection(Connection* socket) {
    connect(socket, SIGNAL(IncomingMessage(QString, QString)), this, SLOT(ReadMessage(QString, QString)));
    connect(socket, SIGNAL(Ready()), this, SLOT(Ready()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
}

void Client::ReadMessage(QString nickname, QString message) {
    emit IncomingMessage(nickname, message);
}

void Client::Disconnected() {
    auto a = qobject_cast<Connection*>(sender());
    RemoveConnection(a);
}

void Client::Error(QAbstractSocket::SocketError error) {
    auto a = qobject_cast<Connection*>(sender());
    m_logger->WriteToLog(Logger::Warning, QString("%1 Connection Error: %2").arg(a->GetNickname()).arg(error));
    RemoveConnection(a);
}

void Client::RemoveConnection(Connection* connection) {
    for (auto p : m_usersconnected.values()) {
        if (!p->peerAddress().isNull() && connection != nullptr) {
            if (p->peerAddress().isEqual(connection->peerAddress())) {
                connection->deleteLater();
                m_usersconnected.remove(connection->peerAddress(),connection);
                emit UserLeft(connection->GetNickname());
            }
        }
    }
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
    socket->SetNickname(QString("abdu: %1").arg(socket->peerPort()));
    emit UserJoined(socket->GetNickname());
}

void Client::UpdateAddresses() {
    for (auto p : QNetworkInterface::allInterfaces()) {
        for (auto q : p.addressEntries()) {
            if (q.broadcast() != QHostAddress::Null && q.ip() != QHostAddress::LocalHost) {
                m_broadcastaddresses.append(q.broadcast());
                m_ipaddresses.append(q.ip());
            }
        }
    }
}

bool Client::IsLocalHostAddress(QHostAddress address) {
    for(auto p : GetIpAddresses()){
        if (address.isEqual(p))
            return true;
    }
    return false;
}

Client::~Client() {
}

}//end client

namespace Network {
Connection::Connection (QObject* parent) : QTcpSocket(parent), m_writer(this), m_reader(this) {
    m_transfertimerid = -1;
    m_pingtimer.setInterval(5000);
    m_type = Undefined;
    m_state = Unconnected;

    QObject::connect(this, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    QObject::connect(&m_pingtimer, SIGNAL(timeout()), this, SLOT(SendPing()));
    QObject::connect(this, SIGNAL(disconnected()), &m_pingtimer, SLOT(stop()));
    QObject::connect(this, SIGNAL(connected()), this, SLOT(ReadyForUse()));
}

Connection::Connection(qintptr descriptor, QObject* parent) : Connection(parent) {
    setSocketDescriptor(descriptor);
    m_reader.setDevice(this);
}

void Connection::ReadyForUse() {
    m_writer.startArray();
    m_pingtimer.start();
    m_pongtimer.start();
    if (!m_reader.device())
        m_reader.setDevice(this);
    emit Ready();
}

void Connection::ReadMessage() {
    m_reader.reparse();

    switch (m_reader.type()) {
    case QCborStreamReader::Type::Map:
    case QCborStreamReader::Type::Array:
        m_reader.enterContainer();
        while (m_reader.lastError() == QCborError::NoError)
            ReadMessage();
        if (m_reader.lastError() == QCborError::NoError)
            m_reader.leaveContainer();
        break;
    case QCborStreamReader::Type::UnsignedInteger:
        m_type = DataType(m_reader.toInteger());
        m_reader.next();
        break;
    case QCborStreamReader::Type::String: {
        auto r = m_reader.readString();
        while (r.status == QCborStreamReader::Ok) {
           m_buffer += r.data;
           r = m_reader.readString();
        }
        m_reader.leaveContainer();
        m_reader.next();
        ProcessMessage();
        break;
    }
    default:
        if (m_reader.isNull()) {
            m_reader.leaveContainer();
            m_reader.next();
            ProcessMessage();
        }
        break;
    }
}

void Connection::ProcessMessage() {
    switch (m_type) {
    case Text:
        emit IncomingMessage(m_nickname, m_buffer);
        break;
    case Ping:
        SendPong();
        break;
    case Pong:
        m_pongtimer.restart();
        break;
    default:
        break;
    }
    m_type = Undefined;
    m_buffer.clear();
}
void Connection::SendPing() {
    if (m_pongtimer.elapsed() > 60000) {
        disconnectFromHost(); //timeout
    }

    m_writer.startMap(1);
    m_writer.append(Ping);
    m_writer.appendNull();
    m_writer.endMap();
}

void Connection::SendPong() {
    m_writer.startMap(1);
    m_writer.append(Pong);
    m_writer.appendNull();
    m_writer.endMap();
}

void Connection::WriteMessage(QString message) {
    if (!message.isEmpty()) {
        m_writer.startMap(1);
        m_writer.append(Text);
        m_writer.append(message);
        m_writer.endMap();
    }
}

Connection::~Connection() {
    if (state() != QTcpSocket::UnconnectedState) {
        m_writer.endArray();
        m_reader.leaveContainer();
        waitForBytesWritten(2000);
    }
}
}
