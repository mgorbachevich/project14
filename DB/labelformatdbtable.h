#ifndef LABELFORMATDBTABLE_H
#define LABELFORMATDBTABLE_H

#include "dbtable.h"

class LabelFormatDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        Name,
        Value,
        COLUMN_COUNT
    };

    LabelFormatDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ LabelFormatDBTable::LabelFormatDBTable";

        addColumn("Код",          "code",  "INT PRIMARY KEY");
        addColumn("Наименование", "name",  "TEXT");
        addColumn("Имя файла",    "value", "TEXT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // LABELFORMATDBTABLE_H
