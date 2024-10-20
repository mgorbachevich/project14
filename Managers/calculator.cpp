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

double Calculator::quantityAsDouble(const DBRecord& product, EquipmentManager* const em)
{
    if(isPiece(product)) return (double)(em->getStatus().pieces);
    if(em->isWMError()) return 0;
    return em->getWeight() * (is100gBase(product) ? 10 : 1);
}

double Calculator::priceAsDouble(const DBRecord& product, const int field)
{
    return DBTable::normalize(product, field).toDouble() / moneyMultiplier;
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
            (double)(DBTable::normalize(product, ProductDBTable::Price).toLongLong()) / moneyMultiplier;
    return Tools::roundToString(v, moneyPointPosition);
}

QString Calculator::unitWeight(const DBRecord& product)
{
    double v = DBTable::normalize(product, ProductDBTable::UnitWeight).toDouble() / 1000; // г
    return Tools::roundToString(v, 3); // г
}

double Calculator::tare(const DBRecord& product)
{
    if (product.count() >= ProductDBTable::Tare) return product[ProductDBTable::Tare].toDouble();
    return 0.0;
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

