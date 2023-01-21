#ifndef RESOURCEDBTABLE_H
#define RESOURCEDBTABLE_H

#include "dbtable.h"

class ResourceDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        Hash,
        Value,
        COLUMN_COUNT
    };

    ResourceDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ ResourceDBTable::ResourceDBTable";

        addColumn("Код",      "code",  "INT PRIMARY KEY");
        addColumn("Хэш",      "hash",  "TEXT");
        addColumn("Значение", "value", "TEXT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // RESOURCEDBTABLE_H
