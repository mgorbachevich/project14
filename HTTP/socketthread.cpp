#include <QTcpSocket>
#include <QDebug>
#include "socketthread.h"

SocketThread::SocketThread(qintptr descriptor, QObject *parent) : QThread(parent), socketDescriptor(descriptor)
{
    qDebug() << "@@@@@ SocketThread::SocketThread";
}

SocketThread::~SocketThread()
{
    qDebug() << "@@@@@ SocketThread::~SocketThread";
    delete socket;
}

void SocketThread::run()
{
    qDebug() << "@@@@@ SocketThread::run";

    socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &SocketThread::onReadyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &SocketThread::onDisconnected, Qt::DirectConnection);
    exec();
}

void SocketThread::onReadyRead()
{
    qDebug() << "@@@@@ SocketThread::onReadyRead request: " << socket->readAll();
    QString response = "Hello! Project14!"; // todo
    QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
    qDebug() << "@@@@@ SocketThread::onReadyRead response: " << head.arg(response).toLatin1();
    socket->write(head.arg(response).toLatin1());
    socket->disconnectFromHost();
}

void SocketThread::onDisconnected()
{
    qDebug() << "@@@@@ SocketThread::onDisconnected";
    socket->close();
    quit();
}
