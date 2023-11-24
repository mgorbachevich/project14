#include "settings.h"
#include "tools.h"

QString Settings::getItemStringValue(const SettingDBTable::SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r != nullptr ? (r->at(SettingDBTable::Value)).toString() : "";
}

int Settings::getItemIntValue(const SettingDBTable::SettingCode code)
{
    return Tools::stringToInt(getItemStringValue(code));
}

bool Settings::getItemBoolValue(const SettingDBTable::SettingCode code)
{
    return getItemIntValue(code) != 0;
}

DBRecord* Settings::getItemByCode(const int code)
{
    bool ok = false;
    for (int i = 0; i < items.count(); i++)
    {
        DBRecord& ri = items[i];
        if (ri[SettingDBTable::Code].toInt(&ok) == code && ok) return &ri;
    }
    return nullptr;
}

DBRecord* Settings::getItemByIndex(const int index)
{
    return (index >= 0 && index < items.count()) ? &items[index] : nullptr;
}

DBRecord *Settings::getItemByIndexInCurrentGroup(const int indexInGroup)
{
    QList<int> currentGroupItemCodes = getCurrentGroupItemCodes();
    int ii = 0;
    for (int i = 0; i < currentGroupItemCodes.count(); i++)
    {
        DBRecord* ri = getItemByCode(currentGroupItemCodes[i]);
        if (ii == indexInGroup) return ri;
        ii++;
    }
    return nullptr;
}

QList<int> Settings::getCurrentGroupItemCodes()
{
    QList<int> codes;
    for (int i = 0; i < items.count(); i++)
    {
        DBRecord& item = items[i];
        int groupCode = item[SettingDBTable::GroupCode].toInt();
        if (groupCode == currentGroupCode) codes.append(item[SettingDBTable::Code].toInt());
    }
    qDebug() << "@@@@@ SettingsPanelModel::getCurrentGroupItemCodes " << codes;
    return codes;
}

DBRecordList Settings::getCurrentGroupItems()
{
    DBRecordList items;
    for (int i = 0; i < items.count(); i++)
    {
        DBRecord& item = items[i];
        int groupCode = item[SettingDBTable::GroupCode].toInt();
        if (groupCode == currentGroupCode) items.append(item);
    }
    return items;
}

bool Settings::isGroup(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Type && r[SettingDBTable::Type].toInt() == SettingDBTable::SettingType_Group;
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getItemByCode(currentGroupCode);
    return (r != nullptr && r->count() >= SettingDBTable::Name)? r->at(SettingDBTable::Name).toString() : "Настройки";
}

void Settings::updateAllItems(const DBRecordList& records)
{
    items.clear();
    items.append(records);
}


