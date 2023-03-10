#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "settingdbtable.h"
#include "settinggroupdbtable.h"
#include "constants.h"

class Settings
{
public:
    Settings() {}
    DBRecord* getItemByIndex(const int index) { return getByIndex(items, index); }
    DBRecord* getItemByCode(const int code);
    DBRecord* getGroupByIndex(const int index) { return getByIndex(groups, index); }
    DBRecord * getItemByIndexInGroup(const int);
    QString getItemStringValue(const SettingDBTable::SettingCode code);
    QList<int> getCurrentGroupItemCodes();
    int getItemIntValue(const SettingDBTable::SettingCode code);
    void createGroups(SettingGroupDBTable*);
    void updateAllItems(const DBRecordList&);

    DBRecordList items;
    DBRecordList groups;
    int currentGroupIndex = 0;

private:
    QList<int> parseGroupItemCodes(DBRecord*);
    DBRecord* getByIndex(DBRecordList&, const int);

};

#endif // SETTINGS_H
