#include "transactiondbtable.h"
#include "tools.h"

DBRecord TransactionDBTable::createRecord(const int userCode,
                                          const int itemCode,
                                          const int labelNumber,
                                          const double weight,
                                          const int price,
                                          const int cost)
{
    DBRecord r = DBTable::createRecord();
    r[TransactionDBTable::DateTime] = Tools::currentDateTimeToInt();
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

