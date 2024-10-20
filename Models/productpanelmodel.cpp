#include "productpanelmodel.h"
#include "productdbtable.h"
#include "tools.h"
#include "calculator.h"
#include "validity.h"

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
            if(v.toInt() > 0 && Calculator::isPiece(product)) text = Calculator::unitWeight(product) + " кг";
            break;

        case ProductDBTable::Price2:
            if(v.toInt() > 0) text = Calculator::price2(product) + " руб/кг";
            break;

        case ProductDBTable::Tare:
            if(v.toDouble() > 0) text = Tools::toString(v.toDouble(), 3) + " кг";
            break;
        /*
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
        case ProductDBTable::ProduceDate: // ниже
        case ProductDBTable::PackingDate: // ниже
        */
        default: break;
        }
        if(!text.isEmpty()) ss << QString("%1: %2").arg(table->columnTitle(i), text);
    }
    Validity v(product);
    ss << v.makeInfo();

    setStringList(ss);
}

