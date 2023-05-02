#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include "database.h"

#define WAIT_FOR_RESPONSE_MSEC 10000
#define WAIT_FOR_RESPONSE_SLEEP_MSEC 10

class QTcpSocket;
class TCPServer;

class SocketThread : public QThread
{
    Q_OBJECT

public:
    explicit SocketThread(QObject*, qintptr, DataBase*);
    ~SocketThread();
    void run();

private:
    enum State
    {
        Disconnected = 0,
        Get,
        Post,
    };
    void waitForDBResponse();

    qintptr socketDescriptor;
    QTcpSocket* socket = nullptr;
    DataBase* db = nullptr;
    QString response = "";
    QString request = "";
    State state = Disconnected;

signals:
    void selectFromDB(const DataBase::Selector, const QString&);
    void download(const DataBase::Selector, const QString&);

public slots:
    void onReadyRead();
    void onDisconnected();
    void onAboutToClose();
    void onDBResult(const DataBase::Selector, const DBRecordList&, const bool);
};

#endif // SOCKETTHREAD_H
