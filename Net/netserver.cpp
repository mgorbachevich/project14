#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "netserver.h"
#include "requestparser.h"
#include "tools.h"
#include "appmanager.h"
#include "externalmessager.h"

NetServer::NetServer(AppManager* parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ NetServer::NetServer");
    parser = new RequestParser(parent);
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
                QString response = parser->parseGetRequest(NetAction_Delete, request.query().toString().toUtf8());
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
                QString response = parser->parseGetRequest(NetAction_Upload, request.query().toString().toUtf8());
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
                QString response = parser->parseSetRequest(request.body());
                Tools::debugLog("@@@@@ NetServer::start setData response " + response);
                emit action(NetAction_DownloadFinished);
                return QHttpServerResponse(response);
            });
        });
        server->route("/action", [this] (const QHttpServerRequest &request)
        {
            return QtConcurrent::run([&request, this]
            {
                emit action(NetAction_Command);
                QString response = parser->parseSetRequest(request.body());
                Tools::debugLog("@@@@@ NetServer::start action response " + response);
                emit action(NetAction_CommandFinished);
                return QHttpServerResponse(response);
            });
        });
    }
}




