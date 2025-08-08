#ifndef RESOURCEDBTABLE_H
#define RESOURCEDBTABLE_H

#include "dbtable.h"

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

    explicit ResourceDBTable(const QString& name, QSqlDatabase& sqlDB, QObject *parent);
    int columnCount() { return COLUMN_COUNT; }
};

#endif // RESOURCEDBTABLE_H
