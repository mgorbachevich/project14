#include "showcasedbtable.h"

bool ShowcaseDBTable::checkRecordForShow(const DBRecord& record)
{
    if (record[ProductDBTable::Columns::Type].toInt() == ProductDBTable::ProductType::ProductType_Group)

}
