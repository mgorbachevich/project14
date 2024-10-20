#ifndef VALIDITY_H
#define VALIDITY_H

#include <QDateTime>
#include "constants.h"

enum ValidityIndex
{
    ValidityIndex_Produce,
    ValidityIndex_Packing,
    ValidityIndex_Sell,
    ValidityIndex_Valid,
    ValidityIndex_Life,
    ValidityIndex_COUNT,
};

struct ValidityItem
{
    explicit ValidityItem() {}

    QString title;
    QString text;
    QDateTime dateTime;
    int value = 0;
};

class Validity
{
public:
    explicit Validity(const DBRecord&);
    QStringList makeInfo();
    QString getText(const ValidityIndex);

private:
    QList<ValidityItem> items;
};

#endif // VALIDITY_H
