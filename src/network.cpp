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
    QObject::connect(&server, SIGNAL(NewConnection(Connection*)), this, SLOT(NewConnection(Connection*)));
}

void Client::SendMessage(const QString& message) {
    auto container = new MessageContainer();
    container->text = message.toStdString();
}

void Client::NewConnection(Connection* connection) {

}

void Client::ConnectionError(QAbstractSocket::SocketError error) {

}

void Client::Disconnected() {

}

void Client::Ready() {

}

Connection::Connection(QObject *parent) :
    QTcpSocket(parent),
{
    m_state = NotConnected;
}

Connection::Connection(qintptr socketdesc, QObject* parent) :
    Connection(parent) {
    setSocketDescriptor(socketdesc);
}

bool Connection::SendMessage(const MessageContainer &message) {

    MessageContainer container = std::move(message);
    return true;
}

Connection::~Connection() {

}
} //namespace Network
