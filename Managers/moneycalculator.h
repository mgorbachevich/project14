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
    double price(const DBRecord&);
    double amount(const DBRecord&);
    double quantity(const DBRecord&);
    QString amountAsString(const DBRecord&);
    QString priceAsString(const DBRecord&);
    QString quantityAsString(const DBRecord&);

private:
    AppManager* appManager;
};

#endif // MONEYCALCULATOR_H
