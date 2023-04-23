#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include "netparams.h"

class TCPServer : public QTcpServer
{
    Q_OBJECT

public:
    TCPServer(QObject *parent): QTcpServer(parent) {}
    void start(const int);
    static NetParams getNetParams();

protected:
    void incomingConnection(qintptr);

signals:
    void showMessageBox(const QString&, const QString&);
    void log(const int, const QString&);
};

#endif // HTTPSERVER_H
