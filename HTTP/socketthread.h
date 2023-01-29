#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>

class QTcpSocket;

class SocketThread : public QThread
{
    Q_OBJECT

public:
    explicit SocketThread(qintptr, QObject *parent = nullptr);
    ~SocketThread();
    void run();

private:
    qintptr socketDescriptor;
    QTcpSocket* socket;

public slots:
    void onReadyRead();
    void onDisconnected();
};

#endif // SOCKETTHREAD_H
