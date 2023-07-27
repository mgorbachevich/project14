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
    qDebug() << "@@@@@ NetServer::start";
    stop();
    server = new QHttpServer();
    if(server->listen(QHostAddress::Any, port) != 0)
        qDebug() << "@@@@@ NetServer::start: listening...";
    else
        qDebug() << "@@@@@ NetServer::start: ERROR";

    server->route("/getData", [this] (const QHttpServerRequest &request) // GET
    {
        const qint64 id = Tools::currentDateTimeToInt(); // id запроса, чтобы не перепутать
        const QString data = request.query().toString(); // Данные запроса
        qDebug() << QString("@@@@@ NetServer::start: route query \n%1").arg(data);
        emit netRequest(RequestType::GET, NetReply(id, data));
        return waitForReplyAndMakeResponse(id);
    });
    server->route("/setData", [this] (const QHttpServerRequest &request) // POST
    {
        const qint64 id = Tools::currentDateTimeToInt(); // id запроса, чтобы не перепутать
        const QString data = request.query().toString(); // Данные запроса
        qDebug() << QString("@@@@@ NetServer::start: route body \n%1").arg(data);
        emit netRequest(RequestType::POST, NetReply(id, data));
        return waitForReplyAndMakeResponse(id);
    });
}

void NetServer::addReply(const NetReply& r)
{
    // Готов ответ БД на запрос клиента. Добавляем в список готовых ответов

    qDebug() << "@@@@@ NetServer::addReply: " << r.first << r.second;
    replies.append(r);
}

QHttpServerResponse NetServer::waitForReplyAndMakeResponse(qint64 id)
{
    // Дожидаемся ответа от БД на запрос и формируем ответ клиенту

    int now = Tools::currentDateTimeToInt();
    bool done = false;
    QString response;
    while (!done) // Wait for reply (SERVER_WAIT_FOR_REPLY_MSEC)
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
    qDebug() << QString("@@@@@ NetServer::waitForReplyAndMakeResponse: %1 \n%2").arg(QString::number(id), response);
    QHttpServerResponse r(response);
    r.setHeader("Content-Type", "application/json");
    return r;
}


