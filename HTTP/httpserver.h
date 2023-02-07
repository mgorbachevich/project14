#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>

class HTTPServer : public QTcpServer
{
    Q_OBJECT

public:
    HTTPServer(QObject *parent, const int port);

    int port = 0;

protected:
    void incomingConnection(qintptr);

signals:
    void showMessageBox(const QString&, const QString&);
};

#endif // HTTPSERVER_H
