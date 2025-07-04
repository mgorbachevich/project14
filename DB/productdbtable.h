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
        ShelfLife,
        SellDate,
        ProduceDate,
        PackingDate,
        PictureCode,
        MessageCode,
        MessageFileCode,
        MovieCode,
        SoundCode,
        Favorite, // Не используется
        COLUMN_COUNT
    };

    ProductDBTable(const QString&, QSqlDatabase&, DataBase*);
    const DBRecord checkRecord(const DBRecord&);
    int columnCount() { return Columns::COLUMN_COUNT; }
    DBRecord makeRootGroup();
};

#endif // PRODUCTDBTABLE_H
