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

    if(product.count() < ProductDBTable::ShelfLife ||
            product.count() < ProductDBTable::SellDate ||
            product.count() < ProductDBTable::ProduceDate||
            product.count() < ProductDBTable::PackingDate)
        return ss;

    const int life    = product[ProductDBTable::ShelfLife].toInt();
    QDateTime sell    = Tools::dateTimeFromString(product[ProductDBTable::SellDate]);
    QDateTime produce = Tools::dateTimeFromString(product[ProductDBTable::ProduceDate]);
    QDateTime packing = Tools::dateTimeFromString(product[ProductDBTable::PackingDate]);

    if(!sell.isValid()) sell = Tools::now();
    if(!produce.isValid()) produce = sell;
    if(!packing.isValid()) packing = produce;

#ifdef CHECK_SELL_DATE
    if(produce > sell || packing < produce || packing > sell) return ss;
#endif

    QString s1 = titles ? "Дата: " : "";
    QString s2 = titles ? "Годен до: " : "";
    QString s3 = titles ? "Годность: " : "";
    s1 += sell.toString(DATE_TIME_SHORT_FORMAT);
    ss << s1;
    if(life > 0)
    {
        s2 += Tools::addDateTime(produce, life, 0).toString(DATE_TIME_SHORT_FORMAT);
        s3 += "дней " + Tools::toString(life);
        ss << s2 << s3;
    }
    else if(life < 0)
    {
        s2 += Tools::addDateTime(produce, 0, -life).toString(DATE_TIME_SHORT_FORMAT);
        s3 += "часов " + Tools::toString(-life);
        ss << s2 << s3;
    }
    return ss;
}
