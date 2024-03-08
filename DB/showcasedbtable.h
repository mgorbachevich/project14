#ifndef SHOWCASEDBTABLE_H
#define SHOWCASEDBTABLE_H

#include "dbtable.h"
#include "tools.h"

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
        Tools::debugLog("@@@@@ ShowcaseDBTable::ShowcaseDBTable");
        addColumn("Код", "code", "TEXT PRIMARY KEY");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // SHOWCASEDBTABLE_H
