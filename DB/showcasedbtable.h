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

    ShowcaseDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ ShowcaseDBTable::ShowcaseDBTable";

        addColumn("Код", "code", "TEXT PRIMARY KEY");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // SHOWCASEDBTABLE_H
