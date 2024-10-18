#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include "constants.h"
#include "productdbtable.h"

class AppManager;

class Calculator : public QObject
{
    Q_OBJECT

public:
    explicit Calculator(AppManager*);
    void update();
    int moneyPointPosition();
    int weightPointPosition();
    QString amountAsString(const DBRecord&);
    QString priceAsString(const DBRecord&, const int field = ProductDBTable::Price);
    QString quantityAsString(const DBRecord&);
    QString weightAsString(const DBRecord&, const int);
    QStringList validity(const DBRecord&, const bool titles = true);

    int moneyMultiplier = 100;
    int weightMultiplier = 1000;

private:
    QVariant normalize(const DBRecord&, const int field);
    double quantityAsDouble(const DBRecord&);

    AppManager* appManager;
};

#endif // CALCULATOR_H
