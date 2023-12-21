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

void NetServer::start(const int port)
{
    qDebug() << "@@@@@ NetServer::start " << port;
    stop();
    server = new QHttpServer();
    if(server->listen(QHostAddress::Any, port) == 0)
        qDebug() << "@@@@@ NetServer::start: ERROR";
    else
    {
        server->route("/deleteData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                emit action(NetAction_Delete);
                QByteArray ba = request.query().toString().toUtf8();
                QString response = RequestParser::parseDeleteRequest(db, ba);
                qDebug() << "@@@@@ NetServer::start deleteData response = " << response;
                emit action(NetAction_DeleteFinished);
                return QHttpServerResponse(response);
            });
        });
        server->route("/getData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                emit action(NetAction_Upload);
                QByteArray ba = request.query().toString().toUtf8();
                QString response = RequestParser::parseGetRequest(db, ba);
                qDebug() << "@@@@@ NetServer::start getData response = " << response;
                emit action(NetAction_UploadFinished);
                return QHttpServerResponse(response);
            });
        });
        server->route("/setData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                emit action(NetAction_Download);
                QByteArray ba = request.body();
                QString response = RequestParser::parseSetRequest(db, ba);
                qDebug() << "@@@@@ NetServer::start setData response =" << response;
                emit action(NetAction_DownloadFinished);
                return QHttpServerResponse(response);
            });
        });
    }
}




