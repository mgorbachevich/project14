#include "settings.h"
#include "tools.h"
#include "settingdbtable.h"
#include "appinfo.h"
#include "database.h"

QString Settings::getItemStringValue(const SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r != nullptr ? (r->at(SettingDBTable::Value)).toString() : "";
}

int Settings::getItemIntValue(const SettingCode code)
{
    return Tools::stringToInt(getItemStringValue(code));
}

bool Settings::getItemBoolValue(const SettingCode code)
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

bool Settings::isGroup(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Type && r[SettingDBTable::Type].toInt() == SettingType_Group;
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getItemByCode(currentGroupCode);
    return (r != nullptr && r->count() >= SettingDBTable::Name)? r->at(SettingDBTable::Name).toString() : "Настройки";
}

void Settings::update(const DBRecordList& records)
{
    items.clear();
    items.append(records);
}

void Settings::setItemValue(const int itemCode, const QString& value)
{
    for (int i = 0; i < items.count(); i++)
    {
        if (items[i][SettingDBTable::Code].toInt() == itemCode)
        {
            items[i][SettingDBTable::Value] = value;
            return;
        }
    }
}

int Settings::getBoudrate(const int code)
{
    switch (code)
    {
    case WMBaudrate_2400:   return 2400;
    case WMBaudrate_4800:   return 4800;
    case WMBaudrate_9600:   return 9600;
    case WMBaudrate_19200:  return 19200;
    case WMBaudrate_38400:  return 38400;
    case WMBaudrate_57600:  return 57600;
    case WMBaudrate_115200: return 115200;
    }
    return 9600;
}


