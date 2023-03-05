#ifndef SETTINGGROUPDBTABLE_H
#define SETTINGGROUPDBTABLE_H

#include "dbtable.h"

class SettingGroupDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0, // enum
        Name,
        Items,
        COLUMN_COUNT
    };

    SettingGroupDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ SettingGroupDBTable::SettingGroupDBTable";

        addColumn("Код",          "code",  "INT PRIMARY KEY");
        addColumn("Наименование", "name",  "TEXT");
        addColumn("Значения",     "items", "TEXT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // SETTINGGROUPDBTABLE_H
