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

const DBRecord* DBTable::checkRecord(const DBRecord& record)
{
    return record.count() >= columnCount() ? &record : nullptr;
}

const DBRecordList DBTable::checkAll(const DBRecordList &records)
{
    DBRecordList resultRecords;
    for (int i = 0; i < records.count(); i++)
    {
        const DBRecord* r = checkRecord(records.at(i));
        if (r != nullptr) resultRecords.append(*r);
    }
    return resultRecords;
}


