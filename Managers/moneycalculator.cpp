#include "moneycalculator.h"
#include "appmanager.h"
#include "productdbtable.h"
#include "settings.h"
#include "equipmentmanager.h"
#include "tools.h"

#define MONEY_POINT_POSITION (appManager->settings->getIntValue(SettingCode_PointPosition))
#define WEIGHT_POINT_POSITION 3

MoneyCalculator::MoneyCalculator(AppManager* parent) : QObject{parent}, appManager(parent) {}

int MoneyCalculator::moneyMultiplier()
{
    int v = 1;
    for(int i = 0; i < MONEY_POINT_POSITION; i++) v *= 10;
    return v;
}

QVariant MoneyCalculator::price(const DBRecord& product)
{
    const int p = ProductDBTable::Price;
    return product.count() <= p ? QVariant(0) : product[p];
}

double MoneyCalculator::quantityAsDouble(const DBRecord& product)
{
    if(ProductDBTable::isPiece(product)) return (double)(appManager->printStatus.pieces);
    if(appManager->equipmentManager->isWMError()) return 0;
    return appManager->equipmentManager->getWeight() * (ProductDBTable::is100gBase(product) ? 10 : 1);
}

QString MoneyCalculator::quantityAsString(const DBRecord& product)
{
    double q = quantityAsDouble(product);
    if(ProductDBTable::isPiece(product))
    {
        double v = Tools::round(q, 0);
        return Tools::doubleToString(v, 0);
    }
    if(appManager->equipmentManager->isWMError()) return "";
    double v = Tools::round(q, WEIGHT_POINT_POSITION);
    return Tools::doubleToString(v, WEIGHT_POINT_POSITION);
}

QString MoneyCalculator::priceAsString(const DBRecord& product)
{
    double p = price(product).toDouble() / moneyMultiplier();
    double v = Tools::round(p, MONEY_POINT_POSITION);
    return Tools::doubleToString(v, MONEY_POINT_POSITION);
}

QString MoneyCalculator::amountAsString(const DBRecord& product)
{
    double a = quantityAsDouble(product) * (double)(price(product).toLongLong()) / moneyMultiplier();
    double v = Tools::round(a, MONEY_POINT_POSITION);
    return Tools::doubleToString(v, MONEY_POINT_POSITION);
}


