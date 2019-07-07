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
    QObject::connect(&server,SIGNAL(NewConnection(Connection*)),this, SLOT(NewConnection(Connection*)));
}

void Client::SendMessage(const QString& message) {

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
    m_cborreader(this),
    m_cborwriter(this)
{
    m_state = NotConnected;
}

Connection::Connection(qintptr socketdesc, QObject* parent) :
    Connection(parent) {
    setSocketDescriptor(socketdesc);
    m_cborreader.setDevice(this);
}

bool Connection::SendMessage(const MessageContainer &message) {
    if (message.text.isEmpty() && message.image.size() == 0) {return false;}

    MessageContainer container = std::move(message);

    for (QImage image: container.image) {
        if (image.isNull()) {
            container.image.erase(container.image.begin());
        }
    }

    uint totalamountofitems = 0;

    if (!container.text.isEmpty()) {totalamountofitems++;}
    totalamountofitems += container.image.size();

    m_cborwriter.startMap(totalamountofitems);
    m_cborwriter.append(Text);
    m_cborwriter.append(container.text.toLatin1());

    for (QImage image: container.image) {
        QByteArray array;
        QBuffer buffer(&array);
        image.save(&buffer);
        m_cborwriter.append(Image);
        m_cborwriter.append(array);
    }

    m_cborwriter.endMap();
    return true;
}

Connection::~Connection() {

}
} //namespace Network
