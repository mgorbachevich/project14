#ifndef PRODUCTDBTABLE_H
#define PRODUCTDBTABLE_H

#include "dbtable.h"

class ProductDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        Barcode,
        Name,
        Name2,
        Code2,
        Type, // enum
        Price,
        PriceBase, // enum
        Price2,
        DiscountCode,
        GroupCode,
        UnitWeight,
        LabelFormat,
        LabelFormat2,
        LabelFormat3,
        BarcodeFormat,
        Tare,
        Certificate,
        Shelflife,
        SellDate,
        ProduceDate,
        PackingDate,
        PictureCode,
        MessageCode,
        MessageFileCode,
        MovieCode,
        SoundCode,
        COLUMN_COUNT
    };

    enum ProductType
    {
        ProductType_Weight = 0,
        ProductType_Piece = 1,
        ProductType_Group = 2
    };

    enum ProductPriceBase
    {
        ProductPriceBase_Kg = 0,
        ProductPriceBase_Piece = 1,
        ProductPriceBase_100g = 2
    };

    ProductDBTable(const QString&, QObject*);
    int columnCount() { return Columns::COLUMN_COUNT; }
    static bool isSuitableForShowcase(const DBRecord&);
    static bool isGroup(const DBRecord&);
};

#endif // PRODUCTDBTABLE_H
