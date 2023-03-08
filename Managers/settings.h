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
    QString getItemStringValue(const SettingDBTable::SettingCode code);
    int getItemIntValue(const SettingDBTable::SettingCode code);
    QList<int> parseGroupItemCodes(DBRecord*);
    void createGroups(SettingGroupDBTable*);
    DBRecordList* updateItems(const DBRecordList&);

    DBRecordList items;
    DBRecordList groups;

private:
    DBRecord* getByIndex(DBRecordList&, const int);

};

#endif // SETTINGS_H
