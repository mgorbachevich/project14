#ifndef NETSERVER_H
#define NETSERVER_H

#define SSL

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>

#ifdef SSL
#include <QSslCertificate>
#include <QSslKey>
#endif

#define SERVER_VERSION "1.4"

class DataBase;

class NetServer : public QObject
{
    Q_OBJECT

public:
    explicit NetServer(QObject*, DataBase*);
    ~NetServer() { stop(); }
    void start(const int);
    void stop();
    QString version() { return SERVER_VERSION; }
    bool isStarted() { return server != nullptr; }

protected:
    QHttpServer* server = nullptr;
    DataBase* db = nullptr;

signals:
    void action(const int);
};

#endif // NETSERVER_H
