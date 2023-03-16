#include <QThread>
#include "constants.h"
#include "net.h"
#include "appmanager.h"
#ifdef HTTP_SERVER
#include "httpserver.h"
#endif
#ifdef HTTP_CLIENT
#include "httpclient.h"
#endif

void Net::startServer(const int port)
{
#ifdef HTTP_SERVER
    if (server == nullptr || server->port != port) // Start or restart
    {
        stopServer();
        if (server == nullptr)
        {
            qDebug() << "@@@@@ Net::startServer port:" << port;
            server = new HTTPServer(nullptr, port);
            connect(server, &HTTPServer::showMessageBox, appManager, &AppManager::showMessageBox);
            connect(server, &HTTPServer::log, appManager, &AppManager::onLog);
        }
    }
#endif
}

void Net::stopServer()
{
#ifdef HTTP_SERVER
    if (server != nullptr)
    {
        qDebug() << "@@@@@ Net::stopServer";
        //disconnect(server, &HTTPServer::showMessageBox, appManager, &AppManager::showMessageBox);
        //disconnect(server, &HTTPServer::log, appManager, &AppManager::onLog);
        delete server;
        server = nullptr;
    }
#endif
}

void Net::startClient(DataBase *db)
{
#if defined(HTTP_CLIENT) || defined(HTTP_SERVER)    // Поддержка SSL:
    // https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ Net.startClient. Device supports OpenSSL:" << QSslSocket::supportsSsl();
#endif

#ifdef HTTP_CLIENT
    if (clientThread == nullptr)
    {
        qDebug() << "@@@@@ Net::startClient";
        clientThread = new QThread();
        HTTPClient* client = new HTTPClient();
        client->moveToThread(clientThread);
        connect(clientThread, &QThread::finished, client, &QObject::deleteLater);
        connect(client, &HTTPClient::newData, db, &DataBase::onNewData);
        connect(client, &HTTPClient::showMessageBox, appManager, &AppManager::showMessageBox);
        connect(client, &HTTPClient::log, appManager, &AppManager::onLog);
        connect(appManager, &AppManager::sendHTTPClientGet, client, &HTTPClient::onSendGet);
        clientThread->start();
    }
#endif
}

void Net::stopClient()
{
#ifdef HTTP_CLIENT
    if (clientThread != nullptr)
    {
        qDebug() << "@@@@@ Net::stopClient";
        clientThread->quit();
        clientThread->wait();
        delete clientThread;
        clientThread = nullptr;
    }
#endif
}
