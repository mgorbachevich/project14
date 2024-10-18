#include "productpanelmodel.h"
#include "productdbtable.h"
#include "tools.h"
#include "appmanager.h"

void ProductPanelModel::update(const DBRecord& product, ProductDBTable* table)
{
    Tools::debugLog("@@@@@ ProductPanelModel::update " + product.at(ProductDBTable::Code).toString());
    QStringList ss;
    for (int i = 0; i < product.count() && i < table->columnCount(); i++)
    {
        QString text;
        const QVariant& v = product.at(i);
        switch(i)
        {
        case ProductDBTable::Code:
        case ProductDBTable::Code2:
        case ProductDBTable::Barcode:
        case ProductDBTable::Name2:
        case ProductDBTable::Certificate:
        case ProductDBTable::ProduceDate:
        case ProductDBTable::PackingDate:
            text = v.toString();
            break;

        case ProductDBTable::Type:
            switch (v.toInt())
            {
            case ProductType_Weight: text =  "Весовой"; break;
            case ProductType_Piece: text =  "Штучный"; break;
            }
            break;

        case ProductDBTable::UnitWeight:
            if(v.toInt() > 0 && ProductDBTable::isPiece(product))
                text = appManager->calculator->weightAsString(product, i) + " кг";
            break;

        case ProductDBTable::Price2:
            if(v.toInt() > 0) text = appManager->calculator->priceAsString(product, i) + " руб/кг";
            break;

        case ProductDBTable::Tare:
            if(v.toDouble() > 0)
                text = Tools::toString(v.toDouble(), appManager->calculator->weightPointPosition()) + " кг";
            break;

        case ProductDBTable::Name: // Рисуется отдельно
        case ProductDBTable::Price: // Рисуется отдельно
        case ProductDBTable::DiscountCode:
        case ProductDBTable::PictureCode:
        case ProductDBTable::MessageCode:
        case ProductDBTable::MessageFileCode:
        case ProductDBTable::MovieCode:
        case ProductDBTable::SoundCode:
        case ProductDBTable::Favorite:
        case ProductDBTable::PriceBase:
        case ProductDBTable::UpperName:
        case ProductDBTable::GroupCode:
        case ProductDBTable::LabelFormat:
        case ProductDBTable::LabelFormat2:
        case ProductDBTable::LabelFormat3:
        case ProductDBTable::BarcodeFormat: // todo
        case ProductDBTable::ShelfLife: // ниже
        case ProductDBTable::SellDate: // ниже
            break;

        default: break;
        }
        if(!text.isEmpty()) ss << QString("%1: %2").arg(table->columnTitle(i), text);
    }
    ss << appManager->calculator->validity(product);
    setStringList(ss);
}

