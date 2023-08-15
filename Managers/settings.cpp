#include "settings.h"
#include "tools.h"
#include "jsonparser.h"

DBRecord* Settings::getItemByCode(const int code)
{
    bool ok = false;
    for (int i = 0; i < items.count(); i++)
    {
        DBRecord& ri = items[i];
        if (ri[SettingDBTable::Code].toInt(&ok) == code && ok)
            return &ri;
    }
    return nullptr;
}

DBRecord *Settings::getItemByIndexInGroup(const int indexInGroup)
{
    QList<int> currentGroupItemCodes = getCurrentGroupItemCodes();
    int j = 0;
    for (int i = 0; i < currentGroupItemCodes.count(); i++)
    {
        DBRecord* ri = getItemByCode(currentGroupItemCodes[i]);
        if(ri != nullptr)
        {
            if (j == indexInGroup)
                return ri;
            j++;
        }
    }
    return nullptr;
}

QString Settings::getItemStringValue(const SettingDBTable::SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r != nullptr ? (r->at(SettingDBTable::Value)).toString() : "";
}

QList<int> Settings::getCurrentGroupItemCodes()
{
    QList<int> codes;
    DBRecord* group = getGroupByIndex(currentGroupIndex);
    if(group != nullptr)
    {
        QStringList sl = group->at(SettingGroupDBTable::Items).toString().split(',');
        for (int i = 0; i < sl.count(); i++)
            codes.append(Tools::stringToInt(sl[i]));
    }
    return codes;
}

int Settings::getItemIntValue(const SettingDBTable::SettingCode code)
{
    return Tools::stringToInt(getItemStringValue(code));
}

bool Settings::getItemBoolValue(const SettingDBTable::SettingCode code)
{
    return getItemIntValue(code) != 0;
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getGroupByIndex(currentGroupIndex);
    if (r != nullptr)
        return r->at(SettingGroupDBTable::Name).toString();
    return "";
}

void Settings::createGroups(SettingGroupDBTable* table)
{
    JSONParser parser;
    groups.clear();
    groups.append(parser.parseTable(table, Tools::readTextFile(DEFAULT_SETTING_GROUPS_FILE)));
}

void Settings::updateAllItems(const DBRecordList& records)
{
    items.clear();
    items.append(records);
}

DBRecord *Settings::getByIndex(DBRecordList& records, const int index)
{
    return (index >= 0 && index < records.count()) ? &records[index] : nullptr;
}
