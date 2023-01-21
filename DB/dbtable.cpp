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

bool DBTable::checkRecordForInsert(const DBRecord& record)
{
    return record.count() >= columnCount();
}

