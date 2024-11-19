#include "validity.h"
#include "tools.h"
#include "productdbtable.h"

Validity::Validity(const DBRecord& product)
{
    for (int i = 0; i < ValidityIndex_COUNT; i++) items << ValidityItem();
    ValidityItem& produce = items[ValidityIndex_Produce];
    ValidityItem& packing = items[ValidityIndex_Packing];
    ValidityItem& sell    = items[ValidityIndex_Sell];
    ValidityItem& valid   = items[ValidityIndex_Valid];
    ValidityItem& life    = items[ValidityIndex_Life];

    produce.title = "Произведено";
    packing.title = "Упаковано";
    sell.title    = "Дата реализации";
    valid.title   = "Годен до";
    life.title    = "Годность";

    produce.dateTime = Tools::dateTimeFromString(DBTable::normalize(product, ProductDBTable::ProduceDate).toString());
    packing.dateTime = Tools::dateTimeFromString(DBTable::normalize(product, ProductDBTable::PackingDate).toString());
    sell.dateTime    = Tools::dateTimeFromString(DBTable::normalize(product, ProductDBTable::SellDate).toString());
    life.value       = DBTable::normalize(product, ProductDBTable::ShelfLife).toInt();

    if(!sell.dateTime.isValid())    sell.dateTime    = Tools::now();
    if(!produce.dateTime.isValid()) produce.dateTime = sell.dateTime;
    if(!packing.dateTime.isValid()) packing.dateTime = produce.dateTime;

    if(life.value > 0)
    {
        valid.dateTime = Tools::addDateTime(produce.dateTime, life.value, 0);
        life.text = Tools::toString(life.value) + " дн";
    }
    else if(life.value < 0)
    {
        valid.dateTime = Tools::addDateTime(produce.dateTime, 0, -life.value);
        life.text = Tools::toString(-life.value) + " ч";
    }
    else
    {
        valid.dateTime = sell.dateTime;
        life.text = "0";
    }

    sell.text    = sell.dateTime.toString(DATE_TIME_SHORT_FORMAT);
    produce.text = produce.dateTime.toString(DATE_TIME_SHORT_FORMAT);
    packing.text = packing.dateTime.toString(DATE_TIME_SHORT_FORMAT);
    valid.text   = valid.dateTime.toString(DATE_TIME_SHORT_FORMAT);
}

QStringList Validity::makeInfo()
{
    QStringList ss;
    foreach (ValidityItem item, items) ss << QString("%1: %2").arg(item.title, item.text);
    return ss;
}

QString Validity::getText(const ValidityIndex i)
{
    if(i >= items.count()) return "";
    return items[i].text;
}

