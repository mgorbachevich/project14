#include "productpanelmodel.h"
#include "productdbtable.h"

void ProductPanelModel::update(const DBRecord& product, ProductDBTable* productTable)
{
    qDebug() << "@@@@@ ProductPanelModel::update " << product.at(ProductDBTable::Code).toString();
    QStringList ss;
    ss << "<b>" + product.at(ProductDBTable::Name).toString() +"</b>" << "";
    for (int i = 0; i < product.count() && i < productTable->columnCount(); i++)
    {
        QString s = productTable->columnTitle(i) + ":  ";
        switch(i)
        {
        case ProductDBTable::Code:
        case ProductDBTable::Barcode:
        case ProductDBTable::Price:
        case ProductDBTable::Shelflife:
        case ProductDBTable::SellDate:
        case ProductDBTable::ProduceDate:
        case ProductDBTable::PackingDate:
        case ProductDBTable::Tare:
        case ProductDBTable::Certificate:
            s += product.at(i).toString();
            break;
        case ProductDBTable::Type:
            switch (product.at(ProductDBTable::Type).toInt())
            {
            case ProductDBTable::ProductType_Weight: s += "Весовой"; break;
            case ProductDBTable::ProductType_Piece: s += "Штучный"; break;
            default: continue;
            }
            break;
        case ProductDBTable::PriceBase:
            switch (product.at(ProductDBTable::PriceBase).toInt())
            {
            case ProductDBTable::ProductPriceBase_Kg: s += "1 кг"; break;
            case ProductDBTable::ProductPriceBase_100g: s += "100 г"; break;
            case ProductDBTable::ProductPriceBase_Piece: s += "Штуки"; break;
            default: continue;
            }
            break;
        case ProductDBTable::Name:
        case ProductDBTable::Name2:
        case ProductDBTable::Code2:
        case ProductDBTable::Price2:
        case ProductDBTable::DiscountCode:
        case ProductDBTable::GroupCode:
        case ProductDBTable::UnitWeight:
        case ProductDBTable::LabelFormat:
        case ProductDBTable::LabelFormat2:
        case ProductDBTable::LabelFormat3:
        case ProductDBTable::BarcodeFormat:
        case ProductDBTable::PictureCode:
        case ProductDBTable::MessageCode:
        case ProductDBTable::MessageFileCode:
        case ProductDBTable::MovieCode:
        case ProductDBTable::SoundCode:
            continue;
        }
        ss << s;
    }
    setStringList(ss);
}
