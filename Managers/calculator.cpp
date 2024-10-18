#include "calculator.h"
#include "appmanager.h"

Calculator::Calculator(AppManager* parent) : QObject{parent}, appManager(parent) {}

void Calculator::update()
{
    moneyMultiplier = 1;
    for(int i = 0; i < moneyPointPosition(); i++) moneyMultiplier *= 10;
    weightMultiplier = 1;
    for(int i = 0; i < weightPointPosition(); i++) weightMultiplier *= 10;
}

int Calculator::moneyPointPosition()
{
    return appManager->settings->getIntValue(SettingCode_PointPosition);
}

int Calculator::weightPointPosition()
{
    return 3;
}

QVariant Calculator::normalize(const DBRecord& product, const int field)
{
    return product.count() <= field ? QVariant(0) : product[field];
}

double Calculator::quantityAsDouble(const DBRecord& product)
{
    if(ProductDBTable::isPiece(product)) return (double)(appManager->status.pieces);
    if(appManager->equipmentManager->isWMError()) return 0;
    return appManager->equipmentManager->getWeight() * (ProductDBTable::is100gBase(product) ? 10 : 1);
}

QString Calculator::quantityAsString(const DBRecord& product)
{
    double v = quantityAsDouble(product);
    if(ProductDBTable::isPiece(product)) return Tools::toString(Tools::round(v, 0), 0);
    if(appManager->equipmentManager->isWMError()) return "";
    return Tools::roundToString(v, weightPointPosition());
}

QString Calculator::priceAsString(const DBRecord& product, const int field)
{
    double p = normalize(product, field).toDouble() / moneyMultiplier;
    return Tools::roundToString(p, moneyPointPosition());
}

QString Calculator::amountAsString(const DBRecord& product)
{
    double v = quantityAsDouble(product) *
            (double)(normalize(product, ProductDBTable::Price).toLongLong()) / moneyMultiplier;
    return Tools::roundToString(v, moneyPointPosition());
}

QString Calculator::weightAsString(const DBRecord& product, const int field)
{
    double v = normalize(product, field).toDouble() / weightMultiplier;
    return Tools::roundToString(v, weightPointPosition());
}

QStringList Calculator::validity(const DBRecord& product, const bool titles)
{
    QStringList ss;
    QDateTime sell = Tools::dateTimeFromString(normalize(product, ProductDBTable::SellDate));
    if(!sell.isValid()) sell = Tools::now();
    ss << QString("%1%2").arg(titles ? "Дата: " : "", sell.toString(DATE_TIME_SHORT_FORMAT));

    QDateTime produce = Tools::dateTimeFromString(normalize(product, ProductDBTable::ProduceDate));
    if(!produce.isValid()) produce = sell;

#ifdef CHECK_SELL_DATE
    QDateTime packing = Tools::dateTimeFromString(normalize(product, ProductDBTable::PackingDate));
    if(!packing.isValid()) packing = produce;
    if(produce > sell || packing < produce || packing > sell) return ss;
#endif

    const int life = normalize(product, ProductDBTable::ShelfLife).toInt();
    if(life > 0)
    {
        ss << QString("%1%2").arg(titles ? "Годен до: " : "",
                  Tools::addDateTime(produce, life, 0).toString(DATE_TIME_SHORT_FORMAT));
        ss << QString("%1%2").arg(titles ? "Годность, дни: " : "", Tools::toString(life));
    }
    else if(life < 0)
    {
        ss << QString("%1%2").arg(titles ? "Годен до: " : "",
                  Tools::addDateTime(produce, 0, -life).toString(DATE_TIME_SHORT_FORMAT));
        ss << QString("%1%2").arg(titles ? "Годность, часы: " : "", Tools::toString(-life));
    }
    return ss;
}
