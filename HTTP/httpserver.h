#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class HTTPServer : public QTcpServer
{
    Q_OBJECT

public:
    HTTPServer(QObject *parent, const int port = 80);

protected:
    void incomingConnection(qintptr);

public slots:
    void onReadyRead();
    void onDisconnected();
};

#endif // HTTPSERVER_H
