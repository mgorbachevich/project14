#include "calculator.h"
#include "settings.h"
#include "equipmentmanager.h"
#include "status.h"
#include "productdbtable.h"

int moneyPointPosition = 2;
int moneyMultiplier = 100;

void Calculator::onSettingsChanged(Settings* const settings)
{
    moneyPointPosition = settings->getIntValue(SettingCode_PointPosition);
    moneyMultiplier = 1;
    for (int i = 0; i < moneyPointPosition; i++) moneyMultiplier *= 10;
}

double Calculator::quantityAsDouble(const DBRecord& product)
{
    if(isPiece(product)) return (double)(EquipmentManager::instance().getStatus().pieces);
    if(EquipmentManager::instance().isWMError()) return 0;
#ifdef FIX_20250115_1
    return EquipmentManager::instance().getWeight();
#else
    return EquipmentManager::instance().getWeight() * (is100gBase(product) ? 10 : 1);
#endif
}

QString Calculator::quantity(const DBRecord& product)
{
    double v = quantityAsDouble(product);
    if(isPiece(product)) return Tools::toString(Tools::round(v, 0), 0);
    if(EquipmentManager::instance().isWMError()) return "";
    return Tools::roundToString(v, 3); // г
}

double Calculator::priceAsDouble(const DBRecord& product, const int priceField, const bool base)
{
    return ((base && is100gBase(product)) ? 10.0 : 1.0) *
            DBTable::normalize(product, priceField).toDouble() / moneyMultiplier;
}

QString Calculator::price(const DBRecord& product, const bool base)
{
    return Tools::roundToString(priceAsDouble(product, ProductDBTable::Price, base), moneyPointPosition);
}

QString Calculator::price2(const DBRecord& product, const bool base)
{
    return Tools::roundToString(priceAsDouble(product, ProductDBTable::Price2, base), moneyPointPosition);
}

QString Calculator::amountAsString(const DBRecord& product, const int field)
{
#ifdef FIX_20250115_1
    double v = quantityAsDouble(product) *
            (is100gBase(product) ? 10.0 : 1.0) *
            (double)(DBTable::normalize(product, field).toLongLong()) / moneyMultiplier;
#else
    double v = quantityAsDouble(product, em) *
            (double)(DBTable::normalize(product, field).toLongLong()) / moneyMultiplier;
#endif
    return Tools::roundToString(v, moneyPointPosition);
}

QString Calculator::amount(const DBRecord& product)
{
    return amountAsString(product, ProductDBTable::Price);
}

QString Calculator::amount2(const DBRecord& product)
{
    return amountAsString(product, ProductDBTable::Price2);
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
#ifdef FIX_20250526_2
        if (n >= v.length())  result = v.rightJustified(n, '0', true);
#else
        result = v.rightJustified(n, '0', true);
#endif
    }
    Tools::debugLog(QString("@@@@@ Calculator::parseBarcode template=%1 char=%2 value=%3 length=%4 result=%5").arg(
                        barcodeTemplate, c, value, Tools::toString(n), result));
    return result;
}

QString Calculator::makeBarcode(Settings* settings, const DBRecord& product, const QString& quantity, const QString& amount)
{
    if (settings == nullptr) return "";
    QString bt = product[ProductDBTable::BarcodeFormat].toString().toUpper(); // Barcode template
    if (bt.size() < 2)
        bt = Calculator::isPiece(product) ?
                settings->getStringValue(SettingCode_PrintLabelBarcodePiece).toUpper() :
                settings->getStringValue(SettingCode_PrintLabelBarcodeWeight).toUpper();
    if (bt.size() < 2) bt = "BBBBBBBBBBBB";
    QStringList order;
    for (int i = 0; i < bt.length(); i++)
    {
        QString c = bt[i];
        if (!order.contains(c)) order << c;
    }
    QString result;
    for (int i = 0; i < order.length(); i++)
    {
        if(order[i] == BARCODE_CHAR_PREFIX)
        {
            QString s = Calculator::isPiece(product) ?
                    settings->getStringValue(SettingCode_PrintLabelPrefixPiece).toUpper() :
                    settings->getStringValue(SettingCode_PrintLabelPrefixWeight).toUpper();
            result += s.rightJustified(2, '0', true);
        }
        else if(order[i] == BARCODE_CHAR_BARCODE)
#ifdef FIX_20250115_2
        {
            QString s = product[ProductDBTable::Barcode].toString();
            for(int i = 0; i < bt.length(); i++)
                if(bt.at(i) == BARCODE_CHAR_BARCODE) result.append(i < s.length() ? s.at(i) : '0');
        }
#else
            result += parseBarcodeValue(bt, BARCODE_CHAR_BARCODE, product[ProductDBTable::Barcode].toString());
#endif
        else if(order[i] == BARCODE_CHAR_CODE)
            result += parseBarcode(bt, BARCODE_CHAR_CODE, product[ProductDBTable::Code].toString());
        else if(order[i] == BARCODE_CHAR_AMOUNT)
            result += parseBarcode(bt, BARCODE_CHAR_AMOUNT, amount);
        else if(order[i] == BARCODE_CHAR_QUANTITY)
            result += parseBarcode(bt, BARCODE_CHAR_QUANTITY, quantity);
    }
    Tools::debugLog(QString("@@@@@ Calculator::makeBarcode template=%1 result=%2").arg(bt, result));
    return result.length() != bt.length() ? "" : result;
}

