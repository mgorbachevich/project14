#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include "database.h"

#define WAIT_FOR_RESPONSE_SEC 10

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
    void waitForResponse();

    qintptr socketDescriptor;
    QTcpSocket* socket = nullptr;
    DataBase* db = nullptr;
    QString response = "";
    bool post = false;

signals:
    void selectFromDB(const DataBase::Selector, const QString&);
    void download(const DataBase::Selector, const QString&);

public slots:
    void onReadyRead();
    void onDisconnected();
    void onSelectFromDBResult(const DataBase::Selector, const DBRecordList&);
    void onUpdateResult(const DataBase::Selector, const bool);
};

#endif // SOCKETTHREAD_H
