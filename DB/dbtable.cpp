#include "dbtable.h"

QVariantList DBTable::createRecord()
{
    DBRecord r;
    for (int i = 0; i < columnCount(); i++)
        r << QVariant("");
    return r;
}

void DBTable::addColumn(const QString& title, const QString& name, const QString& type)
{
    qDebug() << "@@@@@ DBTable::addColumn " << name;
    columns.append(*new DBTableColumn(title, name, type));
}

const DBRecord DBTable::checkRecord(const DBRecord& record)
{
    // qDebug() << "@@@@@ DBTable::checkRecord: table =" << name;
    DBRecord result;
    if(record.count() >= columnCount())
        result.append(record);
    else
        qDebug() << "@@@@@ DBTable::checkRecord ERROR";
    return result;
}

bool DBTable::isEqual(const DBRecord& r1, const DBRecord& r2)
{
    if (r1.count() == r2.count())
    {
        for (int i = 0; i < r1.count(); i++)
        {
            if (r1[i] != r2[i])
                return false;
        }
        return true;
    }
    return false;
}

const DBRecordList DBTable::checkList(const DBRecordList &records)
{
    qDebug() << "@@@@@ DBTable::checkList: table =" << name;
    DBRecordList result;
    for (int i = 0; i < records.count(); i++)
    {
        const DBRecord& ri = checkRecord(records[i]);
        if (!ri.isEmpty())
            result.append(ri);
    }
    return result;
}

QString DBTable::toJsonString(DBTable *table, const DBRecord &record)
{
    QString json;
    for (int i = 0; i < record.count() && i < table->columnCount(); i++)
    {
        if (i > 0)
            json += ",";
        json += QString("\"%1\":\"%2\"").arg(table->columnName(i), record.at(i).toString());
    }
    return "{" + json + "}";
}

QString DBTable::toJsonString(DBTable *table, const DBRecordList &records)
{
    QString json;
    for (int i = 0; i < records.count(); i++)
    {
        if (i > 0)
            json += ",";
        json += toJsonString(table, records.at(i));
    }
    return "[" + json + "]";
}


