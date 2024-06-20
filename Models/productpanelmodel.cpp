#include "productpanelmodel.h"
#include "productdbtable.h"
#include "tools.h"

void ProductPanelModel::update(const DBRecord& product, const QString& price, ProductDBTable* productTable)
{
    Tools::debugLog("@@@@@ ProductPanelModel::update " + product.at(ProductDBTable::Code).toString());
    QStringList ss;
    for (int i = 0; i < product.count() && i < productTable->columnCount(); i++)
    {
        QString value;
        switch(i)
        {
        case ProductDBTable::Code:
        case ProductDBTable::Code2:
        case ProductDBTable::Barcode:
        case ProductDBTable::Shelflife:
        case ProductDBTable::SellDate:
        case ProductDBTable::ProduceDate:
        case ProductDBTable::PackingDate:
        case ProductDBTable::Tare:
        case ProductDBTable::Certificate:
            value = product.at(i).toString();
            break;
        case ProductDBTable::Price:
            value = price;
            break;
        case ProductDBTable::Type:
            switch (product.at(ProductDBTable::Type).toInt())
            {
            case ProductType_Weight: value =  "Весовой"; break;
            case ProductType_Piece: value =  "Штучный"; break;
            }
            break;
        case ProductDBTable::PriceBase:
            if(ProductDBTable::isPiece(product)) continue;
            switch (product.at(ProductDBTable::PriceBase).toInt())
            {
            case ProductPriceBase_Kg: value = "1 кг"; break;
            case ProductPriceBase_100g: value = "100 г"; break;
            }
            break;
        case ProductDBTable::UnitWeight:
            if(ProductDBTable::isPiece(product))
                value = QString("%1 кг").arg(product.at(i).toDouble() / 1000);
            break;
        }
        if(!value.isEmpty()) ss << QString("%1: %2").arg(productTable->columnTitle(i), value);
    }
    setStringList(ss);
}
