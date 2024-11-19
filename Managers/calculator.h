#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "constants.h"

class Settings;
class EquipmentManager;
class Validity;

class Calculator
{
public:
    static void onSettingsChanged(Settings* const);
    static QString amount(const DBRecord&, EquipmentManager* const);
    static QString amount2(const DBRecord&, EquipmentManager* const);
    static QString price(const DBRecord&);
    static QString price2(const DBRecord&);
    static QString quantity(const DBRecord&, EquipmentManager* const);
    static QString unitWeight(const DBRecord&);
    static double tare(const DBRecord&);
    static bool is100gBase(const DBRecord&);
    static bool isGroup(const DBRecord&);
    static bool isPiece(const DBRecord&);
    static bool hasTare(const DBRecord&);
    static int getMoneyPointPosition();
    static QString makeBarcode(Settings*, const DBRecord&, const QString&, const QString&);

private:
    static QString amount(const DBRecord&, EquipmentManager* const, const int);
    static QString parseBarcode(const QString&, const QChar, const QString&);
    static double quantityAsDouble(const DBRecord&, EquipmentManager* const);
    static double priceAsDouble(const DBRecord&, const int);
};

#endif // CALCULATOR_H
