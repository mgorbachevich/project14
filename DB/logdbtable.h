#ifndef LOGDBTABLE_H
#define LOGDBTABLE_H

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

    LogDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ LogDBTable::LogDBTable";

        //addColumn("Дата, время", "date_time", "BIGINT PRIMARY KEY");
        addColumn("Дата, время", "date_time", "UNSIGNED BIG INT PRIMARY KEY");
        addColumn("Тип",         "type",      "INT");
        addColumn("Источник",    "source",    "INT");
        addColumn("Сообщение",   "comment",   "TEXT");
      }

    int columnCount() { return Columns::COLUMN_COUNT; }
    DBRecord createRecord(const int type, const int source, const QString &comment);
};

#endif // LOGDBTABLE_H
