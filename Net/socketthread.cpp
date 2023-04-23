#include <QTcpSocket>
#include <QDebug>
#include "socketthread.h"

SocketThread::SocketThread(qintptr descriptor, QObject *parent) : QThread(parent), socketDescriptor(descriptor)
{
    qDebug() << "@@@@@ SocketThread::SocketThread " << descriptor;
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
    //qDebug() << "@@@@@ SocketThread::onReadyRead: " << socket->bytesAvailable();
    QString s;
    while (socket->bytesAvailable() > 0)
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: " << socket->bytesAvailable();
        s += QString(socket->readAll());
    }
    qDebug() << "@@@@@ SocketThread::onReadyRead: " << s;


    /*
    qDebug() << "@@@@@ SocketThread::onReadyRead: " << QString(socket->readAll());
    QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
    QString body = "Hello! Project14!"; // todo
    QString response = head.arg(body);
    qDebug() << "@@@@@ SocketThread::onReadyRead response: " << response;
    socket->write(response.toLatin1());
    socket->disconnectFromHost();
    */
}

void SocketThread::onDisconnected()
{
    qDebug() << "@@@@@ SocketThread::onDisconnected";
    socket->close();
    quit();
}
