#include <QTimerEvent>

#include "network.h"

namespace  {
static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 60 * 1000;
static const qint32 BroadcastInterval = 2000;
static const unsigned broadcastPort = 45000;
}

//Server class
namespace Network {
Server::Server(QObject* parent) : QTcpServer(parent) {
    listen(QHostAddress::Any);
}

void Server::incomingConnection(qintptr socketDescriptor) {
    Connection* connection = new Connection(socketDescriptor, this);
    emit NewConnection(connection);
}
}//end server

//Connection class
namespace Network {

Connection::Connection(QObject *parent) : QTcpSocket(parent), m_writer(this) {
    m_state = WaitingForGreeting;
    m_greetingmessage = "undefined";
    m_nickname = "lol";
    m_currentdatatype = Undefined;
    m_isgreetingsent = false;
    m_transfertimerid = -1;
    m_timer.setInterval(5000);

    connect(this, SIGNAL(readyRead()), this, SLOT(ProcessReadyRead()));
    connect(this, SIGNAL(disconnected()), &m_timer, SLOT(stop()));
    connect(this, SIGNAL(connected()), this, SLOT(SendGreeting()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(SendPing()));
}

Connection::Connection(qintptr socketdesc, QObject* parent) : Connection(parent) {
    setSocketDescriptor(socketdesc);
    m_reader.setDevice(this);
}

void Connection::timerEvent(QTimerEvent *timerEvent) {
    if (timerEvent->timerId() == m_transfertimerid) {
        abort();
        killTimer(m_transfertimerid);
        m_transfertimerid = -1;
    }
}

void Connection::SendPing() {
    if (m_elapesdtimer.elapsed() > PongTimeout) {
        abort();
        return;
    }

    m_writer.startMap(1);
    m_writer.append(Ping);
    m_writer.append(nullptr);     // no payload
    m_writer.endMap();
}

bool Connection::SendMessage(const QString &message) {
    m_writer.startMap(1);
    m_writer.append(Text);
    m_writer.append(message);
    m_writer.endMap();
    return true;
}

void Connection::ProcessData() {
    switch (m_currentdatatype) {
    case Text:
        emit NewMessage(m_nickname, m_buffer);
        break;
    case Ping:
        m_writer.startMap(1);
        m_writer.append(Pong);
        m_writer.append(nullptr);     // no payload
        m_writer.endMap();
        break;
    case Pong:
        m_elapesdtimer.restart();
        break;
    default:
        break;
    }

    m_currentdatatype = Undefined;
    m_buffer.clear();
}

void Connection::ProcessReadyRead() {
    m_reader.reparse();
    while (m_reader.lastError() == QCborError::NoError) {
        if (m_state == WaitingForGreeting) {
            if (!m_reader.isArray())
                break;                  // protocol error

            m_reader.enterContainer();    // we'll be in this array forever
            m_state = ReadingGreeting;
        } else if (m_reader.containerDepth() == 1) {
            // Current state: no command read
            // Next state: read command ID
            if (!m_reader.hasNext()) {
                m_reader.leaveContainer();
                disconnectFromHost();
                return;
            }

            if (!m_reader.isMap() || !m_reader.isLengthKnown() || m_reader.length() != 1)
                break;                  // protocol error
            m_reader.enterContainer();
        } else if (m_currentdatatype == Undefined) {
            // Current state: read command ID
            // Next state: read command payload
            if (!m_reader.isInteger())
                break;                  // protocol error
            m_currentdatatype = DataType(m_reader.toInteger());
            m_reader.next();
        } else {
            // Current state: read command payload
            if (m_reader.isString()) {
                auto r = m_reader.readString();
                m_buffer += r.data;
                if (r.status != QCborStreamReader::EndOfString)
                    continue;
            } else if (m_reader.isNull()) {
                m_reader.next();
            } else {
                break;                   // protocol error
            }

            // Next state: no command read
            m_reader.leaveContainer();
            if (m_transfertimerid != -1) {
                killTimer(m_transfertimerid);
                m_transfertimerid = -1;
            }

            if (m_state == ReadingGreeting) {
                if (m_currentdatatype != Greeting)
                    break;              // protocol error
                ProcessGreeting();
            } else {
                ProcessData();
            }
        }
    }

    if (m_reader.lastError() != QCborError::EndOfFile)
        abort();       // parse error

    if (m_transfertimerid != -1 && m_reader.containerDepth() > 1)
        m_transfertimerid = startTimer(TransferTimeout);
}

void Connection::SendGreeting() {
    m_writer.startArray();        // this array never ends

    m_writer.startMap(1);
    m_writer.append(Greeting);
    m_writer.append(m_greetingmessage);
    m_writer.endMap();
    m_isgreetingsent = true;

    if (!m_reader.device())
        m_reader.setDevice(this);
}

void Connection::ProcessGreeting() {
    m_currentdatatype = Undefined;
    m_buffer.clear();

    if (!isValid()) {
        abort();
        return;
    }

    if (!m_isgreetingsent)
        SendGreeting();

    m_timer.start();
    m_elapesdtimer.start();
    m_state = Ready;
    emit ReadyForUse();
}

Connection::~Connection() {
    if (m_isgreetingsent) {
        // Indicate clean shutdown.
        m_writer.endArray();
        waitForBytesWritten(2000);
    }
}
} //end connection

//FriendManager class
namespace Network {
FriendManager::FriendManager(Client* client) : QObject(client), m_client(client) {
    static const char *envVariables[] = {
        "USERNAME", "USER", "USERDOMAIN", "HOSTNAME", "DOMAINNAME"
    };

    for (const char *varname : envVariables) {
        m_nickname = qEnvironmentVariable(varname);
        if (!m_nickname.isNull())
            break;
    }

    if (m_nickname.isEmpty())
        m_nickname = "unknown";

    UpdateAddresses();
    m_serverport = 0;

    m_broadcasttimer.setInterval(BroadcastInterval);
    m_broadcastsocket.bind(QHostAddress::Any, broadcastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    connect(&m_broadcastsocket, SIGNAL(readyRead()), this, SLOT(ReadBroadcastDatagram()));
    connect(&m_broadcasttimer, SIGNAL(timeout()), this, SLOT(SendBroadcastDatagram()));
}

void FriendManager::StartBroadcasting() {
    m_broadcasttimer.start();
}

bool FriendManager::IsLocalHostAddress(const QHostAddress &address) {
    for (const QHostAddress &localAddress : m_ipaddresses) {
        if (address.isEqual(localAddress))
            return true;
    }
    return false;
}

void FriendManager::SendBroadcastDatagram() {
    QByteArray datagram;
    {
        QCborStreamWriter writer(&datagram);
        writer.startArray(2);
        writer.append(m_nickname);
        writer.append(m_serverport);
        writer.endArray();
    }

    bool validBroadcastAddresses = true;
    for (const QHostAddress &address : qAsConst(m_broadcastaddresses)) {
        if (m_broadcastsocket.writeDatagram(datagram, address,
                                          broadcastPort) == -1)
            validBroadcastAddresses = false;
    }

    if (!validBroadcastAddresses)
        UpdateAddresses();
}

void FriendManager::ReadBroadcastDatagram() {
    while (m_broadcastsocket.hasPendingDatagrams()) {
        QHostAddress senderIp;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(m_broadcastsocket.pendingDatagramSize());
        if (m_broadcastsocket.readDatagram(datagram.data(), datagram.size(),
                                         &senderIp, &senderPort) == -1)
            continue;

        int senderServerPort;
        {
            // decode the datagram
            QCborStreamReader reader(datagram);
            if (reader.lastError() != QCborError::NoError || !reader.isArray())
                continue;
            if (!reader.isLengthKnown() || reader.length() != 2)
                continue;

            reader.enterContainer();
            if (reader.lastError() != QCborError::NoError || !reader.isString())
                continue;
            while (reader.readString().status == QCborStreamReader::Ok) {
                // we don't actually need the username right now
            }

            if (reader.lastError() != QCborError::NoError || !reader.isUnsignedInteger())
                continue;
            senderServerPort = reader.toInteger();
        }

        if (IsLocalHostAddress(senderIp) && senderServerPort == m_serverport)
            continue;

        if (!m_client->IsConnected(senderIp)) {
            Connection *connection = new Connection(this);
            emit NewConnection(connection);
            connection->connectToHost(senderIp, senderServerPort);
        }
    }
}

void FriendManager::UpdateAddresses() {
    m_broadcastaddresses.clear();
    m_ipaddresses.clear();

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        const QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
                m_broadcastaddresses << broadcastAddress;
                m_ipaddresses << entry.ip();
            }
        }
    }
}

}//end FriendManager

//Client class
namespace Network {
Client::Client() {
    m_nickname = "abdu";
    m_friendmanager = new FriendManager(this);
    m_friendmanager->SetServerPort(m_server.serverPort());
    m_friendmanager->StartBroadcasting();

    connect(m_friendmanager,SIGNAL(NewConnection(Connection*)), this, SLOT(NewConnection(Connection*)));
    connect(&m_server, SIGNAL(NewConnection(Connection*)), this, SLOT(NewConnection(Connection*)));
}

void Client::NewConnection(Connection* connection) {
    connection->SetGreeting(GetNickname());
    connect(connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ConnectionError(QAbstractSocket::SocketError)));
    connect(connection, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(connection, SIGNAL(ReadyForUse()), this, SLOT(Ready()));
}

void Client::SendMessage(const QString& message) {
    for (Connection *connection : qAsConst(m_friendmap))
        connection->SendMessage(message);
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
    bool t = IsConnected(connection->peerAddress(), connection->peerPort());
    if (connection != nullptr || IsConnected(connection->peerAddress(), connection->peerPort()))
        return;

    connect(connection, SIGNAL(NewMessage(QString,QString)), this, SIGNAL(NewMessage(QString,QString)));

    m_friendmap.insert(connection->peerAddress(), connection);
    QString nick = connection->GetNickname();
    if (!nick.isEmpty())
        emit NewFriend(nick);
}

}//end client
