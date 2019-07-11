#include "network.h"

namespace Network {

Server::Server(QObject* parent) : QTcpServer(parent) {
    listen(QHostAddress::Any);
}

void Server::incomingConnection(qintptr socketDescriptor) {
    Connection* connection = new Connection(socketDescriptor, this);
    emit NewConnection(connection);
}

Client::Client() {

    QObject::connect(&m_server, SIGNAL(NewConnection(Connection*)), this, SLOT(NewConnection(Connection*)));
}

void Client::SendMessage(const QString& message) {
    for (Connection *connection : qAsConst(m_friendmap))
        connection->SendMessage(message);
}

void Client::NewConnection(Connection* connection) {
    connect(connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(connection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(connection, SIGNAL(readyForUse()), this, SLOT(Ready()));
}

bool Client::IsConnected(const QHostAddress &senderIp, int senderPort) {
    if (senderPort == -1)
        return m_friendmap.contains(senderIp);

    if (!m_friendmap.contains(senderIp))
        return false;

    const QList<Connection *> connections = m_friendmap.values(senderIp);
    for (const Connection *connection : connections) {
        if (connection->peerPort() == senderPort)
            return true;
    }

    return false;
}
void Client::ConnectionError(QAbstractSocket::SocketError error) {
    if (Connection *connection = qobject_cast<Connection *>(sender()))
            RemoveConnection(connection);
}

void Client::RemoveConnection(Connection *connection) {
    if (m_friendmap.contains(connection->peerAddress())) {
        m_friendmap.remove(connection->peerAddress());
        QString nick = connection->GetNickname();

        if (!nick.isEmpty())
            emit FriendDisconnected(nick);
    }
    connection->deleteLater();
}
void Client::Disconnected() {
    if (Connection *connection = qobject_cast<Connection *>(sender()))
        RemoveConnection(connection);
}

void Client::Ready() {
    Connection* connection = qobject_cast<Connection*>(sender());
    if (connection != nullptr || IsConnected(connection->peerAddress(), connection->peerPort()))
        return;

    connect(connection, SIGNAL(newMessage(QString,QString)),
            this, SIGNAL(newMessage(QString,QString)));

    m_friendmap.insert(connection->peerAddress(), connection);
    QString nick = connection->GetNickname();
    if (!nick.isEmpty())
        emit NewFriend(nick);
}

Connection::Connection(QObject *parent) : QTcpSocket(parent), m_writer(this) {
    m_state = NotConnected;
    connect(this, SIGNAL(readyRead()), this, SLOT(ProcessReadyRead()));
    connect(this, SIGNAL(disconnect()), &PingTimer, SLOT(stop()));
}

Connection::Connection(qintptr socketdesc, QObject* parent) :
    Connection(parent) {
    setSocketDescriptor(socketdesc);
    m_reader.setDevice(this);
}

bool Connection::SendMessage(const QString &message) {
    m_writer.startMap(1);
    m_writer.append(QcobrPlainText);
    m_writer.append(message);
    m_writer.endMap();
    return true;
}

Connection::~Connection() {

}
} //namespace Network
