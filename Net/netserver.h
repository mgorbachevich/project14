#ifndef NETSERVER_H
#define NETSERVER_H

#define SSL

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include "constants.h"

#ifdef SSL
#include <QSslCertificate>
#include <QSslKey>
#endif

class NetServer : public QObject
{
    Q_OBJECT

public:
    enum RequestType
    {
        GET,
        POST
    };
    explicit NetServer(QObject *parent = nullptr);
    ~NetServer() { if (server != nullptr) delete server; }
    void start(const int);
    void makeReply(const NetReply&);

protected:
    QHttpServerResponse makeResponse(qint64);

    QHttpServer* server = nullptr;
    QList<NetReply> replies;

signals:
    void netRequest(const int, const NetReply&);
};

#endif // NETSERVER_H
