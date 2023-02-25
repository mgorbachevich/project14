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
        Comment,
        COLUMN_COUNT
    };

    enum LogType
    {
        LogType_Error = 0,
        LogType_Warning = 1,
        LogType_Info = 2,
        LogType_Debug = 3,
        LogType_Transaction = 4
    };

    LogDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ LogDBTable::LogDBTable";

        addColumn("Дата, время", "date_time", "BIGINT PRIMARY KEY");
        addColumn("Тип",         "type",      "INT");
        addColumn("Сообщение",   "comment",   "INT");
      }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // LOGDBTABLE_H
