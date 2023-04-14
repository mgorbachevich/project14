#ifndef DBTHREAD_H
#define DBTHREAD_H

#include <QThread>
#include "database.h"

class DBThread : public QThread
{
    Q_OBJECT

public:
    DBThread(DataBase*, QObject*);
    void stop();
};

#endif // DBTHREAD_H
