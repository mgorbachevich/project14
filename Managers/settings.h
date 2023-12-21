#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "constants.h"

class Settings
{
public:
    Settings() {}
    int getItemIntValue(const SettingCode);
    bool getItemBoolValue(const SettingCode);
    QString getItemStringValue(const SettingCode);
    DBRecord* getItemByCode(const int);
    DBRecord* getItemByIndexInCurrentGroup(const int);
    void setItemValue(const int, const QString&);
    QList<int> getCurrentGroupItemCodes();
    QString getCurrentGroupName();
    void update(const DBRecordList&);
    static int getBoudrate(const int);
    static bool isGroup(const DBRecord&);

    int currentGroupCode = 0;

private:
    DBRecordList items;
};

#endif // SETTINGS_H
