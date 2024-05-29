#ifndef SHOWCASEDBTABLE_H
#define SHOWCASEDBTABLE_H

#include "dbtable.h"

class ShowcaseDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        COLUMN_COUNT
    };

    ShowcaseDBTable(const QString&, QObject*);
    int columnCount() { return Columns::COLUMN_COUNT; }
    DBRecord createRecord(const QString&);
};

#endif // SHOWCASEDBTABLE_H
