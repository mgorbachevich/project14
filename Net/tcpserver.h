#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include "netparams.h"

class DataBase;

class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    TCPServer(QObject*, DataBase*);
    void start(const int);
    static NetParams getNetParams();

protected:
    void incomingConnection(qintptr);

    DataBase* db;

signals:
    void showMessageBox(const QString&, const QString&, const bool);
    void log(const int, const QString&);
};

#endif // HTTPSERVER_H
