#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "netserver.h"
#include "requestparser.h"
#include "database.h"

NetServer::NetServer(QObject *parent, DataBase* dataBase): QObject{parent}, db(dataBase)
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

QHttpServerResponse NetServer::getData(const QHttpServerRequest &request)
{
    emit action(NetAction_Upload);
    QByteArray ba = request.query().toString().toUtf8();
    QString response = RequestParser::parseGetRequest(db, ba);
    qDebug() << "@@@@@ NetServer::getData response = " << response;
    emit action(NetAction_UploadFinished);
    return QHttpServerResponse(response);
}

QHttpServerResponse NetServer::deleteData(const QHttpServerRequest &request)
{
    emit action(NetAction_Delete);
    QByteArray ba = request.query().toString().toUtf8();
    QString response = RequestParser::parseDeleteRequest(db, ba);
    qDebug() << "@@@@@ NetServer::deleteData response = " << response;
    emit action(NetAction_DeleteFinished);
    return QHttpServerResponse(response);
}

QHttpServerResponse NetServer::setData(const QHttpServerRequest &request)
{
    emit action(NetAction_Download);
    QByteArray ba = request.body();
    QString response = RequestParser::parseSetRequest(db, ba);
    qDebug() << "@@@@@ NetServer::setData response =" << response;
    emit action(NetAction_DownloadFinished);
    return QHttpServerResponse(response);
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
        server->route("/deleteData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                return deleteData(request);
            });
        });
        server->route("/getData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                return getData(request);
            });
        });
        server->route("/setData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                return setData(request);
            });
        });
    }
}




