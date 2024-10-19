#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include "constants.h"

class Settings;
class EquipmentManager;

class Calculator : public QObject
{
    Q_OBJECT

public:
    explicit Calculator(QObject*);
    static void update(Settings* const);
    static QString amount(const DBRecord&, EquipmentManager* const);
    static QString price(const DBRecord&);
    static QString price2(const DBRecord&);
    static QString quantity(const DBRecord&, EquipmentManager* const);
    static QString unitWeight(const DBRecord&);
    static double tare(const DBRecord&);
    static QStringList validity(const DBRecord&, const bool titles = true);
    static bool is100gBase(const DBRecord&);
    static bool isForShowcase(const DBRecord&);
    static bool isGroup(const DBRecord&);
    static bool isPiece(const DBRecord&);
    static bool hasTare(const DBRecord&);
    static int getMoneyPointPosition();

private:
    static QVariant normalize(const DBRecord&, const int field);
    static double quantityAsDouble(const DBRecord&, EquipmentManager* const);
    static double priceAsDouble(const DBRecord&, const int);
};

#endif // CALCULATOR_H
