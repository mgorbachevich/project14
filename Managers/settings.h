#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "settingdbtable.h"
#include "constants.h"

class Settings
{
public:
    Settings() {}
    int getItemIntValue(const SettingDBTable::SettingCode);
    bool getItemBoolValue(const SettingDBTable::SettingCode);
    QString getItemStringValue(const SettingDBTable::SettingCode);
    DBRecord* getItemByCode(const int);
    DBRecord* getItemByIndex(const int);
    DBRecord* getItemByIndexInCurrentGroup(const int);
    QList<int> getCurrentGroupItemCodes();
    DBRecordList getCurrentGroupItems();
    bool isGroup(const DBRecord&);
    QString getCurrentGroupName();
    void updateAllItems(const DBRecordList&);

    DBRecordList items;
    int currentGroupCode = 0;
};

#endif // SETTINGS_H
