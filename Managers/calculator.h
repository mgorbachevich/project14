#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "constants.h"

class Settings;
class Validity;

class Calculator
{
public:
    static void onSettingsChanged(Settings* const);
    static QString amount(const DBRecord&);
    static QString amount2(const DBRecord&);
    static QString price(const DBRecord&, const bool base = false);
    static QString price2(const DBRecord&, const bool base = false);
    static QString quantity(const DBRecord&);
    static QString unitWeight(const DBRecord&);
    static double tare(const DBRecord&);
    static bool is100gBase(const DBRecord&);
    static bool isGroup(const DBRecord&);
    static bool isPiece(const DBRecord&);
    static bool hasTare(const DBRecord&);
    static int getMoneyPointPosition();
    static QString makeBarcode(Settings*, const DBRecord&, const QString&, const QString&);

private:
    static QString parseBarcode(const QString&, const QChar, const QString&);
    static double quantityAsDouble(const DBRecord&);
    static double priceAsDouble(const DBRecord&, const int, const bool);
    static QString amountAsString(const DBRecord&,const int);
};

#endif // CALCULATOR_H
