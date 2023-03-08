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

QString Settings::getItemStringValue(const SettingDBTable::SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r != nullptr ? (r->at(SettingDBTable::Value)).toString() : "";
}

int Settings::getItemIntValue(const SettingDBTable::SettingCode code)
{
    return Tools::stringToInt(getItemStringValue(code));
}

QList<int> Settings::parseGroupItemCodes(DBRecord* group)
{
    QList<int> codes;
    if(group != nullptr)
    {
        QStringList sl = group->at(SettingGroupDBTable::Items).toString().split(',');
        for (int i = 0; i < sl.count(); i++)
            codes.append(Tools::stringToInt(sl[i]));
    }
    return codes;
}

void Settings::createGroups(SettingGroupDBTable* table)
{
    JSONParser parser;
    groups.clear();
    groups.append(parser.parseTable(table, Tools::readTextFile(DEFAULT_SETTING_GROUPS_FILE)));
#ifdef DEBUG_LOG_SETTING_GROUPS
    for (int i = 0; i < groups.count(); i++)
    {
        QString s = "@@@@@ Settings::createGroups " + groups[i].at(SettingGroupDBTable::Name).toString();
        QList<int> codes = parseGroupItemCodes(&groups[i]);
        for (int j = 0; j < codes.count(); j++)
            s += QString(" %1").arg(codes[j]);
        qDebug() << s;
    }
#endif
}

DBRecordList* Settings::updateItems(const DBRecordList& records)
{
    items.clear();
    items.append(records);
    return &items;
}

DBRecord *Settings::getByIndex(DBRecordList& records, const int index)
{
    return (index >= 0 && index < records.count()) ? &records[index] : nullptr;
}
