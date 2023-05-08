#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include "database.h"

class QTcpSocket;

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
        Upload,
        Download,
    };

    qintptr socketDescriptor;
    QString request = "";
    DBRecordList dbResponse;
    State state = Disconnected;
    DataBase* db;
    QTcpSocket* socket;

signals:
    void selectFromDB(const DataBase::Selector, const QString&, const QString&);
    void download(const QString&);

public slots:
    void onDBRequestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void onReadyRead();
    void onDisconnected();
    void onAboutToClose();
};

#endif // SOCKETTHREAD_H
