#include "calculator.h"
#include "productdbtable.h"
#include "settings.h"
#include "equipmentmanager.h"
#include "status.h"

int moneyPointPosition;
int moneyMultiplier;

Calculator::Calculator(QObject* parent) : QObject{parent}
{
    moneyPointPosition = 2;
    moneyMultiplier = 100;
}

void Calculator::update(Settings* const settings)
{
    moneyPointPosition = settings->getIntValue(SettingCode_PointPosition);
    moneyMultiplier = 1;
    for (int i = 0; i < moneyPointPosition; i++) moneyMultiplier *= 10;
}

QVariant Calculator::normalize(const DBRecord& product, const int field)
{
    return product.count() <= field ? QVariant(0) : product[field];
}

double Calculator::quantityAsDouble(const DBRecord& product, EquipmentManager* const em)
{
    if(isPiece(product)) return (double)(em->getStatus().pieces);
    if(em->isWMError()) return 0;
    return em->getWeight() * (is100gBase(product) ? 10 : 1);
}

double Calculator::priceAsDouble(const DBRecord& product, const int field)
{
    return normalize(product, field).toDouble() / moneyMultiplier;
}

QString Calculator::quantity(const DBRecord& product, EquipmentManager* const em)
{
    double v = quantityAsDouble(product, em);
    if(isPiece(product)) return Tools::toString(Tools::round(v, 0), 0);
    if(em->isWMError()) return "";
    return Tools::roundToString(v, 3); // г
}

QString Calculator::price(const DBRecord& product)
{
    return Tools::roundToString(priceAsDouble(product, ProductDBTable::Price), moneyPointPosition);
}

QString Calculator::price2(const DBRecord& product)
{
    return Tools::roundToString(priceAsDouble(product, ProductDBTable::Price2), moneyPointPosition);
}

QString Calculator::amount(const DBRecord& product, EquipmentManager* const em)
{
    double v = quantityAsDouble(product, em) *
            (double)(normalize(product, ProductDBTable::Price).toLongLong()) / moneyMultiplier;
    return Tools::roundToString(v, moneyPointPosition);
}

QString Calculator::unitWeight(const DBRecord& product)
{
    double v = normalize(product, ProductDBTable::UnitWeight).toDouble() / 1000; // г
    return Tools::roundToString(v, 3); // г
}

double Calculator::tare(const DBRecord& product)
{
    if (product.count() >= ProductDBTable::Tare) return product[ProductDBTable::Tare].toDouble();
    return 0.0;
}

QStringList Calculator::validity(const DBRecord& product, const bool titles)
{
    QStringList ss;
    QDateTime sell = Tools::dateTimeFromString(normalize(product, ProductDBTable::SellDate).toString());
    if(!sell.isValid()) sell = Tools::now();
    QDateTime produce = Tools::dateTimeFromString(normalize(product, ProductDBTable::ProduceDate).toString());
    if(!produce.isValid()) produce = sell;
    QDateTime packing = Tools::dateTimeFromString(normalize(product, ProductDBTable::PackingDate).toString());
    if(!packing.isValid()) packing = produce;

    ss << QString("%1%2").arg(titles ? "Произведено: " : "",     produce.toString(DATE_TIME_SHORT_FORMAT));
    ss << QString("%1%2").arg(titles ? "Упаковано: " : "",       packing.toString(DATE_TIME_SHORT_FORMAT));
    ss << QString("%1%2").arg(titles ? "Дата реализации: " : "", sell.toString(DATE_TIME_SHORT_FORMAT));

#ifdef CHECK_SELL_DATE
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
    else
    {
        ss << QString("%1%2").arg(titles ? "Годен до: " : "",
                sell.toString(DATE_TIME_SHORT_FORMAT));
        ss << QString("%1%2").arg(titles ? "Годность: " : "", "0");
    }
    return ss;
}

bool Calculator::is100gBase(const DBRecord& product)
{
    return (product.count() >= ProductDBTable::PriceBase) &&
            product[ProductDBTable::PriceBase].toInt() == ProductPriceBase_100g;
}

bool Calculator::isForShowcase(const DBRecord& product)
{
    return (product.count() >= ProductDBTable::COLUMN_COUNT) && !isGroup(product);
}

bool Calculator::isGroup(const DBRecord& product)
{
    return (product.count() >= ProductDBTable::Type) &&
            product[ProductDBTable::Type].toInt() == ProductType_Group;
}

bool Calculator::isPiece(const DBRecord& product)
{
    return (product.count() >= ProductDBTable::Type) &&
            product[ProductDBTable::Type].toInt() == ProductType_Piece;
}

bool Calculator::hasTare(const DBRecord& product)
{
    return (product.count() >= ProductDBTable::Tare) &&
            product[ProductDBTable::Tare].toDouble() > 0;
}

int Calculator::getMoneyPointPosition()
{
    return moneyPointPosition;
}

