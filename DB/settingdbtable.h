#ifndef SETTINGDBTABLE_H
#define SETTINGDBTABLE_H

#include "dbtable.h"

class SettingDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0, // enum
        Name,
        Value,
        Text,
        COLUMN_COUNT
    };

    enum SettingCode
    {
        SettingCode_None = 0,
    };

    SettingDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ SettingDBTable::SettingDBTable";

        addColumn("Код",          "code",  "INT PRIMARY KEY");
        addColumn("Наименование", "name",  "TEXT");
        addColumn("Значение",     "value", "INT");
        addColumn("Текст",        "text",  "TEXT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // SETTINGDBTABLE_H
