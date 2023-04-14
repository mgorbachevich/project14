#include "dbthread.h"

DBThread::DBThread(DataBase *db, QObject *parent) : QThread(parent)
{
    db->moveToThread(this);
    connect(this, &QThread::finished, db, &QObject::deleteLater);
}

void DBThread::stop()
{
    quit();
    wait();
}
