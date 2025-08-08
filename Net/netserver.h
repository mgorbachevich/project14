#ifndef NETSERVER_H
#define NETSERVER_H

#define SSL

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include "loner.h"
#include "constants.h"
#include "netactionresult.h"

#ifdef SSL
#include <QSslCertificate>
#include <QSslKey>
#endif

#define SERVER_VERSION "1.5"

class NetServer :  public QObject, public Loner<NetServer>
{
    Q_OBJECT
    friend class Loner<NetServer>;

private:
    explicit NetServer();
    ~NetServer();

public:
    void start(const int);
    void stop();
    QString version() { return SERVER_VERSION; }
    bool isStarted() { return httpServer != nullptr; }

protected:
    NetActionResult parseGetRequest(const RouterRule, const QByteArray&);
    NetActionResult parseSetRequest(const RouterRule, const QByteArray&);
    QString toJsonString(const QByteArray&);
    bool parseCommand(const QByteArray&);
    QString onRoute(const RouterRule, const QHttpServerRequest&);
#ifdef FIX_20250704_1
    QByteArray parseHeaderItem(const QByteArray&, const QByteArray&);
#else
    QByteArray parseHeaderItem(const QByteArray&, const QByteArray&, const QByteArray& title = "Content-Disposition");
#endif
    QHttpServer* httpServer = nullptr;

signals:
    void netCommand(const int, const QString&);
};

#endif // NETSERVER_H
