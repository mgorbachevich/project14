#include "calculator.h"
#include "productdbtable.h"
#include "settings.h"
#include "equipmentmanager.h"
#include "status.h"

int moneyPointPosition = 2;
int moneyMultiplier = 100;

void Calculator::onSettingsChanged(Settings* const settings)
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

QString Calculator::amount(const DBRecord& product, EquipmentManager* const em, const int field)
{
    double v = quantityAsDouble(product, em) *
            (double)(DBTable::normalize(product, field).toLongLong()) / moneyMultiplier;
    return Tools::roundToString(v, moneyPointPosition);
}

QString Calculator::amount(const DBRecord& product, EquipmentManager* const em)
{
    return amount(product, em, ProductDBTable::Price);
}

QString Calculator::amount2(const DBRecord& product, EquipmentManager* const em)
{
    return amount(product, em, ProductDBTable::Price2);
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

QString Calculator::parseBarcode(const QString& barcodeTemplate, const QChar c, const QString& value)
{
    QString result;
    int n = 0;
    for(int i = 0; i < barcodeTemplate.length(); i++) if(barcodeTemplate.at(i) == c) n++;
    if(n > 0)
    {
        if (n > 12) n = 12; // Длина ШК = 12. Контрольный 13-й символ добавляется позже
        QString v = value;
        v = v.replace(QString("."), QString("")).replace(QString(","), QString(""));
        result = v.rightJustified(n, '0', true);
    }
    Tools::debugLog(QString("@@@@@ Calculator::parseBarcode template=%1 char=%2 value=%3 length=%4 result=%5").arg(
                        barcodeTemplate, c, value, Tools::toString(n), result));
    return result;
}

QString Calculator::makeBarcode(Settings* settings, const DBRecord& product, const QString& quantity, const QString& amount)
{
    if (settings == nullptr) return "";
    QString barcodeTemplate = product[ProductDBTable::BarcodeFormat].toString().toUpper();
    if (barcodeTemplate.size() < 2)
        barcodeTemplate = Calculator::isPiece(product) ?
                settings->getStringValue(SettingCode_PrintLabelBarcodePiece).toUpper() :
                settings->getStringValue(SettingCode_PrintLabelBarcodeWeight).toUpper();
    if (barcodeTemplate.size() < 2) barcodeTemplate = "BBBBBBBBBBBB";
    QStringList order;
    for (int i = 0; i < barcodeTemplate.length(); i++)
    {
        QString c = barcodeTemplate[i];
        if (!order.contains(c)) order << c;
    }
    QString result;
    for (int i = 0; i < order.length(); i++)
    {
        if(order[i] == "P")
        {
            QString s = Calculator::isPiece(product) ?
                    settings->getStringValue(SettingCode_PrintLabelPrefixPiece).toUpper() :
                    settings->getStringValue(SettingCode_PrintLabelPrefixWeight).toUpper();
            result += s.rightJustified(2, '0', true);
        }
        else if(order[i] == "C")
            result += parseBarcode(barcodeTemplate, 'C', product[ProductDBTable::Code].toString());
        else if(order[i] == "B")
            result += parseBarcode(barcodeTemplate, 'B', product[ProductDBTable::Barcode].toString());
        else if(order[i] == "T")
            result += parseBarcode(barcodeTemplate, 'T', amount);
        else if(order[i] == "W")
            result += parseBarcode(barcodeTemplate, 'W', quantity);

    }
    Tools::debugLog(QString("@@@@@ Calculator::makeBarcode template=%1 result=%2").arg(barcodeTemplate, result));
    return result.length() != barcodeTemplate.length() ? "" : result;
}

