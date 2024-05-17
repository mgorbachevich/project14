#ifndef MONEYCALCULATOR_H
#define MONEYCALCULATOR_H

#include <QObject>
#include "constants.h"

class AppManager;

class MoneyCalculator : public QObject
{
    Q_OBJECT

public:
    explicit MoneyCalculator(AppManager*);
    QString amountAsString(const DBRecord&);
    QString priceAsString(const DBRecord&);
    QString quantityAsString(const DBRecord&);

private:
    int moneyMultiplier();
    double quantityAsDouble(const DBRecord&);
    QVariant price(const DBRecord&);

    AppManager* appManager;
};

#endif // MONEYCALCULATOR_H
