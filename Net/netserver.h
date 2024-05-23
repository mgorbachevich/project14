#ifndef NETSERVER_H
#define NETSERVER_H

#define SSL

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include "externalmessager.h"

#ifdef SSL
#include <QSslCertificate>
#include <QSslKey>
#endif

#define SERVER_VERSION "1.5"

class AppManager;
class RequestParser;

class NetServer : public ExternalMessager
{
    Q_OBJECT

public:
    explicit NetServer(AppManager*);
    ~NetServer() { stop(); }
    void start(const int);
    void stop();
    QString version() { return SERVER_VERSION; }
    bool isStarted() { return server != nullptr; }

protected:
    QHttpServer* server = nullptr;
    RequestParser* parser = nullptr;

signals:
    void action(const int);
};

#endif // NETSERVER_H
