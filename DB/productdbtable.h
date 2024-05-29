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
        UpperName,
        Code2,
        Type, // enum ProductType
        Price,
        PriceBase, // enum ProductPriceBase
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
        Favorite,
        COLUMN_COUNT
    };

    ProductDBTable(const QString&, QObject*);
    const DBRecord checkRecord(const DBRecord&);
    int columnCount() { return Columns::COLUMN_COUNT; }
    static bool isForShowcase(const DBRecord&);
    static bool isPiece(const DBRecord&);
    static bool is100gBase(const DBRecord&);
    static bool isGroup(const DBRecord&);
};

#endif // PRODUCTDBTABLE_H
