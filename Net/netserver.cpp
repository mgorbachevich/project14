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
                isActive = true;
                emit action(NetAction_Delete);
                QByteArray ba = request.query().toString().toUtf8();
                qDebug() << QString("@@@@@ NetServer::start: deleteData length = %1\n").arg(ba.length());
                QString response = RequestParser::parseDeleteRequest(db, ba);
                qDebug() << QString("@@@@@ NetServer::start: deleteData response = %1\n").arg(response);
                emit action(NetAction_DeleteFinished);
                isActive = false;
                return QHttpServerResponse(response);
            });
        });
        server->route("/getData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                isActive = true;
                emit action(NetAction_Upload);
                QByteArray ba = request.query().toString().toUtf8();
                qDebug() << QString("@@@@@ NetServer::start: getData length = %1\n").arg(ba.length());
                QString response = RequestParser::parseGetRequest(db, ba);
                qDebug() << QString("@@@@@ NetServer::start: getData response = %1\n").arg(response);
                emit action(NetAction_UploadFinished);
                isActive = false;
                return QHttpServerResponse(response);
            });
        });
        server->route("/setData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                isActive = true;
                emit action(NetAction_Download);
                QByteArray ba = request.body();
                qDebug() << QString("@@@@@ NetServer::start: setData length = %1\n").arg(ba.length());
                QString response = RequestParser::parseSetRequest(db, ba);
                qDebug() << QString("@@@@@ NetServer::start: setData response = %1\n").arg(response);
                emit action(NetAction_DownloadFinished);
                isActive = false;
                return QHttpServerResponse(response);
            });
        });
    }
}




