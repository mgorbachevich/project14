#ifndef TRANSACTIONDBTABLE_H
#define TRANSACTIONDBTABLE_H

#include "dbtable.h"

class TransactionDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        DateTime,
        User,
        ItemCode,
        LabelNumber,
        Weight,
        Price,
        Cost,
        Price2,
        Cost2,
        COLUMN_COUNT
    };

    TransactionDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ TransactionDBTable::TransactionDBTable";

        addColumn("Код",              "code",         "INT PRIMARY KEY");
        addColumn("Дата, время",      "date_time",    "DATETIME");
        addColumn("Код пользователя", "user",         "INT");
        addColumn("Код товара",       "item_code",    "INT");
        addColumn("Номер этикетки",   "label_number", "INT");
        addColumn("Вес / количество", "weight",       "REAL");
        addColumn("Цена",             "price",        "INT");
        addColumn("Стоимость",        "cost",         "INT");
        addColumn("Цена 2",           "price2",       "INT");
        addColumn("Стоимость 2",      "cost2",        "INT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
};

#endif // TRANSACTIONDBTABLE_H
