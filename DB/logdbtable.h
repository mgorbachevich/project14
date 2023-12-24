#ifndef LOGDBTABLE_H
#define LOGDBTABLE_H

#include <QDebug>
#include "dbtable.h"

class LogDBTable: public DBTable
{
public:
    enum Columns
    {
        DateTime, // the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
        Type,
        Source,
        Comment,
        COLUMN_COUNT
    };

    LogDBTable(const QString& name, QObject *parent);
    int columnCount() { return Columns::COLUMN_COUNT; }
    DBRecord createRecord(const int type, const int source, const QString &comment);
};

#endif // LOGDBTABLE_H
