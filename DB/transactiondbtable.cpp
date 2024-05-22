#include "transactiondbtable.h"
#include "tools.h"

TransactionDBTable::TransactionDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ TransactionDBTable::TransactionDBTable");

    addColumn("Дата, время",      "date_time",    "UNSIGNED BIG INT PRIMARY KEY");
    addColumn("Код пользователя", "user",         "INT");
    addColumn("Код товара",       "item_code",    "INT");
    addColumn("Номер этикетки",   "label_number", "INT");
    addColumn("Вес/количество",   "weight",       "REAL");
    addColumn("Цена",             "price",        "INT");
    addColumn("Стоимость",        "cost",         "INT");
    addColumn("Цена 2",           "price2",       "INT");
    addColumn("Стоимость 2",      "cost2",        "INT");
}

DBRecord TransactionDBTable::createRecord(const qint64 dateTime,
                                          const int userCode,
                                          const int itemCode,
                                          const int labelNumber,
                                          const double weight,
                                          const int price,
                                          const int cost)
{
    DBRecord r = DBTable::createRecord();
    r[TransactionDBTable::DateTime] = dateTime;
    r[TransactionDBTable::User] = userCode;
    r[TransactionDBTable::ItemCode] = itemCode;
    r[TransactionDBTable::LabelNumber] = labelNumber;
    r[TransactionDBTable::Weight] = weight;
    r[TransactionDBTable::Price] = price;
    r[TransactionDBTable::Cost] = cost;
    r[TransactionDBTable::Price2] = 0; // todo
    r[TransactionDBTable::Cost2] = 0; // todo
    return r;
}

