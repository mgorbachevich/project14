#include <QDebug>
#include "httpserver.h"
#include "socketthread.h"

// https://www.youtube.com/watch?v=ab1GoP_W5vw
// https://www.youtube.com/watch?v=lwvtv9r8uaM
HTTPServer::HTTPServer(QObject *parent, const int port): QTcpServer(parent)
{
    qDebug() << "@@@@@ HTTPServer::HTTPServer";
    if (!listen(QHostAddress::Any, port))
        qDebug() << "@@@@@ HTTPServer::HTTPServer ERROR " << errorString();
}

void HTTPServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "@@@@@ HTTPServer::incomingConnection";
    SocketThread* thread = new SocketThread(socketDescriptor, this);
    connect(thread, &SocketThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

