#include <QDebug>
#include "httpserver.h"
#include "socketthread.h"

// https://www.youtube.com/watch?v=ab1GoP_W5vw
// https://www.youtube.com/watch?v=lwvtv9r8uaM
HTTPServer::HTTPServer(QObject *parent, const int tcpPort): QTcpServer(parent)
{
    qDebug() << "@@@@@ HTTPServer::HTTPServer " << tcpPort;
    port = tcpPort;
    if (!listen(QHostAddress::Any, port))
    {
        qDebug() << "@@@@@ HTTPServer::HTTPServer ERROR " << errorString();
        port = 0;
    }
}

void HTTPServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "@@@@@ HTTPServer::incomingConnection";
    SocketThread* thread = new SocketThread(socketDescriptor, this);
    connect(thread, &SocketThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

