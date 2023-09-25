#include "logdbtable.h"
#include "tools.h"

DBRecord LogDBTable::createRecord(const int type, const int source, const QString &comment)
{
    DBRecord r = DBTable::createRecord();
    r[LogDBTable::DateTime] = Tools::currentDateTimeToUInt();
    r[LogDBTable::Type] = type;
    r[LogDBTable::Source] = source;
    r[LogDBTable::Comment] = comment;
    return r;
}

