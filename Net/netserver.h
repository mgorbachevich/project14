#ifndef NETSERVER_H
#define NETSERVER_H

#define SSL

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include "externalmessager.h"
#include "constants.h"
#include "netactionresult.h"

#ifdef SSL
#include <QSslCertificate>
#include <QSslKey>
#endif

#define SERVER_VERSION "1.5"

class AppManager;

class NetServer : public ExternalMessager
{
    Q_OBJECT

public:
    explicit NetServer(AppManager*);
    ~NetServer();
    void start(const int);
    void stop();
    QString version() { return SERVER_VERSION; }
    bool isStarted() { return server != nullptr; }

protected:
    NetActionResult parseGetRequest(const RouterRule, const QByteArray&);
    NetActionResult parseSetRequest(const RouterRule, const QByteArray&);
    QString toJsonString(const QByteArray&);
    QByteArray parseHeaderItem(const QByteArray&, const QByteArray&, const QByteArray& title = "Content-Disposition");
    bool parseCommand(const QByteArray&);
    QString onRoute(const RouterRule, const QHttpServerRequest&);

    QHttpServer* server = nullptr;

signals:
    void netCommand(const int, const QString&);
};

#endif // NETSERVER_H
