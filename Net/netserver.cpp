#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "netserver.h"
#include "tools.h"

NetServer::NetServer(QObject *parent): QObject{parent}
{
    qDebug() << "@@@@@ NetServer::NetServer";
}

void NetServer::start(const int port)
{
    if (server != nullptr) delete server;
    server = new QHttpServer();
    if(server->listen(QHostAddress::Any, port) != 0)
        qDebug() << "@@@@@ NetServer::start: listening...";
    else
        qDebug() << "@@@@@ NetServer::start: ERROR";

    server->route("/getData", [this] (const QHttpServerRequest &request) // GET
    {
        const QString data = request.query().toString();
        const qint64 id = Tools::currentDateTimeToInt();
        qDebug() << QString("@@@@@ NetServer::start: route query \n%1").arg(data);
        emit netRequest(RequestType::GET, NetReply(id, data));
        return makeResponse(id);
    });
    server->route("/setData", [this] (const QHttpServerRequest &request) // POST
    {
        const QString data = request.body();
        const qint64 id = Tools::currentDateTimeToInt();
        qDebug() << QString("@@@@@ NetServer::start: route body \n%1").arg(data);
        emit netRequest(RequestType::POST, NetReply(id, data));
        return makeResponse(id);
    });
}

void NetServer::makeReply(const NetReply& r)
{
    qDebug() << "@@@@@ NetServer::makeReply: " << r.first << r.second;
    replies.append(r);
}

QHttpServerResponse NetServer::makeResponse(qint64 id)
{
    int now = Tools::currentDateTimeToInt();
    bool done = false;
    QString response;
    while (!done) // Wait for reply
    {
        for (int i = 0; !done && i < replies.count(); i++)
        {
            if(replies[i].first == id)
            {
                response = replies[i].second;
                replies.removeAt(i);
                done = true;
            }
        }
        if (!done && Tools::currentDateTimeToInt() < now + SERVER_WAIT_FOR_REPLY_MSEC)
            QThread::msleep(SERVER_WAIT_FOR_REPLY_SLEEP_MSEC);
    }
    qDebug() << QString("@@@@@ NetServer::makeResponse: %1 \n%2").arg(QString::number(id), response);
    QHttpServerResponse r(response);
    r.setHeader("Content-Type", "application/json");
    return r;
}


