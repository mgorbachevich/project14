#include "logdbtable.h"
#include "tools.h"

LogDBTable::LogDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ LogDBTable::LogDBTable");

    addColumn("Дата, время", "date_time", "UNSIGNED BIG INT PRIMARY KEY");
    addColumn("Тип",         "type",      "INT");
    addColumn("Источник",    "source",    "INT");
    addColumn("Сообщение",   "comment",   "TEXT");
}

DBRecord LogDBTable::createRecord(const int type, const int source, const QString &comment)
{
    DBRecord r = DBTable::createRecord();
    r[LogDBTable::DateTime] = Tools::nowMsec();
    r[LogDBTable::Type] = type;
    r[LogDBTable::Source] = source;
    r[LogDBTable::Comment] = comment;
    return r;
}

