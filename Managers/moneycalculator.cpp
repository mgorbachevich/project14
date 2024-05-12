#include "moneycalculator.h"
#include "appmanager.h"
#include "productdbtable.h"
#include "settings.h"
#include "equipmentmanager.h"
#include "tools.h"

#define POINT_POSITION appManager->settings->getIntValue(SettingCode_PointPosition)

MoneyCalculator::MoneyCalculator(AppManager* parent) : QObject{parent}, appManager(parent) {}

double MoneyCalculator::price(const DBRecord& product)
{
    const int p = ProductDBTable::Price;
    double v = product.count() <= p? 0 : product[p].toString().toDouble();
    for (int i = 0; i < POINT_POSITION; i++) v /= 10;
    return Tools::round(v, POINT_POSITION);
}

double MoneyCalculator::quantity(const DBRecord& product)
{
    if(ProductDBTable::isPiece(product))
        return Tools::round(appManager->printStatus.pieces, 0);
    if(appManager->equipmentManager->isWMError())
        return 0;
    const double w = appManager->equipmentManager->getWeight() * (ProductDBTable::is100gBase(product) ? 10 : 1);
    return Tools::round(w, 3);
}

double MoneyCalculator::amount(const DBRecord& product)
{
    return Tools::round(quantity(product) * price(product), POINT_POSITION);
}

QString MoneyCalculator::priceAsString(const DBRecord& product)
{
    return Tools::doubleToString(price(product), POINT_POSITION);
}

QString MoneyCalculator::amountAsString(const DBRecord& product)
{
    return Tools::doubleToString(amount(product), POINT_POSITION);
}

QString MoneyCalculator::quantityAsString(const DBRecord& product)
{
    if(ProductDBTable::isPiece(product))
        return Tools::doubleToString(quantity(product), 0);
    return appManager->equipmentManager->isWMError() ? "" : Tools::doubleToString(quantity(product), 3);
}

