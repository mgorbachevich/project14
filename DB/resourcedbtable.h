#ifndef RESOURCEDBTABLE_H
#define RESOURCEDBTABLE_H

#include "dbtable.h"
#include "tools.h"

class ResourceDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        Name,
        Value,
        Hash,
        Source,
        COLUMN_COUNT
    };

    ResourceDBTable(const QString& name, QSqlDatabase& sqlDB, DataBase *parent): DBTable(name, sqlDB, parent)
    {
        Tools::debugLog("@@@@@ ResourceDBTable::ResourceDBTable");

        addColumn("Код",          "code",   "UNSIGNED BIG INT PRIMARY KEY");
        addColumn("Наименование", "name",   "TEXT");
        addColumn("Значение",     "value",  "TEXT");
        addColumn("Хэш",          "hash",   "TEXT");
        addColumn("Источник",     "source", "TEXT");
    }
    int columnCount() { return COLUMN_COUNT; }
};

#endif // RESOURCEDBTABLE_H
