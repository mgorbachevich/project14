#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "netserver.h"
#include "tools.h"

NetServer::NetServer(QObject *parent): QObject{parent}
{
    qDebug() << "@@@@@ NetServer::NetServer";
}

void NetServer::stop()
{
    if (server != nullptr)
    {
        qDebug() << "@@@@@ NetServer::stop";
        delete server;
        server = nullptr;
    }
}

void NetServer::start(const int port)
{
    qDebug() << "@@@@@ NetServer::start " << port;
    stop();
    server = new QHttpServer();
    int p = server->listen(QHostAddress::Any, port);
    if(p == 0)
        qDebug() << "@@@@@ NetServer::start: ERROR";
    else
    {
        qDebug() << "@@@@@ NetServer::start: listening...";
        server->route("/getData", [this] (const QHttpServerRequest &request)
        {
            QByteArray ba = request.query().toString().toUtf8();
            //qDebug() << QString("@@@@@ NetServer::start: query \n%1\n").arg(ba);
            const quint64 requestId = Tools::currentDateTimeToUInt();
            emit netRequest(NetRequestType_GetData, NetReplyPair(requestId, ba));
            return waitAndMakeResponse(requestId);
        });
        server->route("/setData", [this] (const QHttpServerRequest &request)
        {
            QByteArray ba = request.body();
            //qDebug() << QString("@@@@@ NetServer::start: body \n%1\n").arg(ba);
            const quint64 requestId = Tools::currentDateTimeToUInt();
            emit netRequest(NetRequestType_SetData, NetReplyPair(requestId, ba));
            return waitAndMakeResponse(requestId);
        });
    }
}

void NetServer::sendReplyToClient(const NetReplyPair& p)
{
    replies.append(p);
}

QHttpServerResponse NetServer::waitAndMakeResponse(quint64 requestId)
{
    quint64 now = Tools::currentDateTimeToUInt();
    bool done = false;
    QString response;
    while (!done)  // Ожидание ответа
    {
        for (int i = 0; !done && i < replies.count(); i++)
        {
            if(replies[i].first == requestId)
            {
                response = replies[i].second;
                replies.removeAt(i);
                done = true;
            }
        }
        if (!done)
        {
            if(Tools::currentDateTimeToUInt() < now + SERVER_WAIT_FOR_REPLY_MSEC)
                QThread::msleep(SERVER_WAIT_FOR_REPLY_SLEEP_MSEC);
            else
            {
                qDebug() << QString("@@@@@ NetServer::waitAndMakeResponse TIMEOUT");
                done = true; // Время ожидания истекло! todo
            }
        }
    }
    qDebug() << QString("@@@@@ NetServer::waitAndMakeResponse: %1 \n%2\n").arg(QString::number(requestId), response);
    return QHttpServerResponse(response);
}



