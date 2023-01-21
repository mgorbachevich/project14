#include "productdbtable.h"

bool ProductDBTable::isSuitableForShowcase(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::Columns::COLUMN_COUNT) && !isGroup(record);
}

bool ProductDBTable::isGroup(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::Columns::COLUMN_COUNT) &&
            record[ProductDBTable::Columns::Type].toInt() == ProductDBTable::ProductType::ProductType_Group;
}
