#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include "netparams.h"
#include "database.h"

class AppManager;

class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    TCPServer(QObject *parent, DataBase* dataBase): QTcpServer(parent), db(dataBase) {}
    void start(const int);
    static NetParams getNetParams();

    DataBase* db;

protected:
    void incomingConnection(qintptr);

signals:
    void showMessageBox(const QString&, const QString&);
    void log(const int, const QString&);
};

#endif // HTTPSERVER_H
