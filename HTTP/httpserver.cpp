#include <QDebug>
#include "httpserver.h"

HTTPServer::HTTPServer(QObject *parent, const int port): QTcpServer(parent)
{
    //https://www.youtube.com/watch?v=ab1GoP_W5vw
    qDebug() << "@@@@@ HTTPServer::HTTPServer";

    if (!listen(QHostAddress::Any, port))
        qDebug() << "@@@@@ HTTPServer::HTTPServer ERROR " << errorString();
}

void HTTPServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &HTTPServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &HTTPServer::onDisconnected);
}

void HTTPServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "@@@@@ HTTPServer::onReadyRead: " << socket->readAll();

    QString response = "Hello!"; // todo
    QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
    socket->write(head.arg(response).toLatin1());
    socket->disconnectFromHost();
}

void HTTPServer::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->close();
    socket->deleteLater();
}
