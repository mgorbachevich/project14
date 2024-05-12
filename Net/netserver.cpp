#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "netserver.h"
#include "requestparser.h"
#include "database.h"
#include "tools.h"
#include "appmanager.h"
#include "externalmessager.h"

NetServer::NetServer(AppManager* parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ NetServer::NetServer");
}

void NetServer::stop()
{
    if (server != nullptr)
    {
        Tools::debugLog("@@@@@ NetServer::stop");
        delete server;
        server = nullptr;
    }
}

void NetServer::start(const int port)
{
    Tools::debugLog("@@@@@ NetServer::start " + QString::number(port));
    stop();
    server = new QHttpServer();
    if(server->listen(QHostAddress::Any, port) == 0)
        Tools::debugLog("@@@@@ NetServer::start ERROR");
    else
    {
        server->route("/deleteData", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                emit action(NetAction_Delete);
                QByteArray ba = request.query().toString().toUtf8();
                QString response = RequestParser::parseDeleteRequest(appManager->db, ba);
                Tools::debugLog("@@@@@ NetServer::start deleteData " + response);
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
                QString response = RequestParser::parseGetRequest(appManager->db, ba);
                Tools::debugLog("@@@@@ NetServer::start getData response " + response);
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
                QString response = RequestParser::parseSetRequest(appManager->db, ba);
                Tools::debugLog("@@@@@ NetServer::start setData response " + response);
                emit action(NetAction_DownloadFinished);
                return QHttpServerResponse(response);
            });
        });
    }
}




