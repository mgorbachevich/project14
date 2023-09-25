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

#define SERVER_VERSION "1.1"

class NetServer : public QObject
{
    Q_OBJECT

public:
    explicit NetServer(QObject *parent = nullptr);
    ~NetServer() { stop(); }
    void start(const int);
    void stop();
    QString version() { return SERVER_VERSION; }
    void sendReplyToClient(const NetReplyPair&);

protected:
    QHttpServerResponse waitAndMakeResponse(quint64);

    QHttpServer* server = nullptr;
    QList<NetReplyPair> replies;

signals:
    void netRequest(const int, const NetReplyPair&);
};

#endif // NETSERVER_H
