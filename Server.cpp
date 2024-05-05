#include "Server.h"
#include <QDataStream>

Server::Server(QObject *parent) : QTcpServer(parent) {
    //connect(this, &Server::newConnection, this, &Server::incomingConnection);
    connect(this, &QTcpServer::newConnection, this, &Server::handleNewConnection);
    //listen(QHostAddress::Any, 1234);
}

/*void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        connect(socket, &QTcpSocket::readyRead, this, &Server::readClient);
        connect(socket, &QTcpSocket::disconnected, this, &Server::disconnectClient);
        clients.insert(socket, QString());
    } else {
        delete socket;
    }
}*/

bool Server::startServer(quint16 port) {
    if (!listen(QHostAddress::Any, port)) {
        qCritical() << "Could not start server on port" << port << ":" << errorString();
        return false;
    }
    qDebug() << "Server started, listening on port" << port;
    return true;
}

void Server::handleNewConnection() {
    while (hasPendingConnections()) {
        QTcpSocket *socket = nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &Server::readClient);
        connect(socket, &QTcpSocket::disconnected, this, &Server::disconnectClient);
        clients.insert(socket, QString());
    }
}

void Server::readClient() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QDataStream in(socket);
    QString playerName;
    bool won;


    if (!in.atEnd()) {
        in >> playerName >> won;

        if (in.status() == QDataStream::Ok) {
            scores[playerName] += (won ? 1 : -1);
            printScoreboard();
            sendScoreboard();
            qDebug()<<"should be updated";
            saveScoresToFile();
        } else {
            qDebug() << "Error reading from socket"<< in.status();;
        }
    } else {
            qDebug() << "No more data to read or incomplete data.";
        }

}

void Server::disconnectClient() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    clients.remove(socket);
    socket->deleteLater();
}

void Server::sendScoreboard() {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << scores;

    for (QTcpSocket *client : clients.keys()) {
        client->write(block);
    }
}
