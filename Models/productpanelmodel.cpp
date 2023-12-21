#include "productpanelmodel.h"
#include "productdbtable.h"
#include "appmanager.h"

void ProductPanelModel::update(const DBRecord& product, ProductDBTable* productTable)
{
    qDebug() << "@@@@@ ProductPanelModel::update " << product.at(ProductDBTable::Code).toString();
    QStringList ss;
    //ss << "<b>" + product.at(ProductDBTable::Name).toString() +"</b>" << "";
    for (int i = 0; i < product.count() && i < productTable->columnCount(); i++)
    {
        QString s = productTable->columnTitle(i) + ":  ";
        switch(i)
        {
        case ProductDBTable::Code:
        case ProductDBTable::Barcode:
        case ProductDBTable::Shelflife:
        case ProductDBTable::SellDate:
        case ProductDBTable::ProduceDate:
        case ProductDBTable::PackingDate:
        case ProductDBTable::Tare:
        case ProductDBTable::Certificate:
            s += product.at(i).toString();
            break;
        case ProductDBTable::Price:
            s += QString("%1").arg(appManager->price(product));
            break;
        case ProductDBTable::Type:
            switch (product.at(ProductDBTable::Type).toInt())
            {
            case ProductType_Weight: s += "Весовой"; break;
            case ProductType_Piece: s += "Штучный"; break;
            default: s = ""; break;
            }
            break;
        case ProductDBTable::PriceBase:
            if(ProductDBTable::isPiece(product))
                s = "";
            else
            {
                switch (product.at(ProductDBTable::PriceBase).toInt())
                {
                case ProductPriceBase_Kg: s += "1 кг"; break;
                case ProductPriceBase_100g: s += "100 г"; break;
                default: s = ""; break;
                }
            }
            break;
        case ProductDBTable::UnitWeight:
            if(ProductDBTable::isPiece(product))
                s += QString("%1 кг").arg(product.at(i).toDouble() / 1000000);
            else
                s = "";
            break;
        case ProductDBTable::Name:
        case ProductDBTable::Name2:
        case ProductDBTable::Code2:
        case ProductDBTable::Price2:
        case ProductDBTable::DiscountCode:
        case ProductDBTable::GroupCode:
        case ProductDBTable::LabelFormat:
        case ProductDBTable::LabelFormat2:
        case ProductDBTable::LabelFormat3:
        case ProductDBTable::BarcodeFormat:
        case ProductDBTable::PictureCode:
        case ProductDBTable::MessageCode:
        case ProductDBTable::MessageFileCode:
        case ProductDBTable::MovieCode:
        case ProductDBTable::SoundCode:
            s = "";
            break;
        }
        if(!s.isEmpty()) ss << s;
    }
    setStringList(ss);
}
