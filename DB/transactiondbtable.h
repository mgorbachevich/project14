#ifndef TRANSACTIONDBTABLE_H
#define TRANSACTIONDBTABLE_H

#include <QDebug>
#include "dbtable.h"

class TransactionDBTable: public DBTable
{
public:
    enum Columns
    {
        DateTime = 0, // the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
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

    TransactionDBTable(const QString& name, QObject *parent);
    int columnCount() { return Columns::COLUMN_COUNT; }
    DBRecord createRecord(const qint64, const int, const int, const int, const double, const int, const int);
};

#endif // TRANSACTIONDBTABLE_H
