#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "constants.h"

class Settings
{
public:
    Settings() {}
    int getItemCode(const DBRecord&);
    int getItemType(const DBRecord&);
    QString getItemName(const DBRecord&);
    int getItemIntValue(const SettingCode);
    int getItemIntValue(const DBRecord&);
    bool getItemBoolValue(const SettingCode);
    QString getItemStringValue(const SettingCode);
    QString getItemStringValue(const DBRecord&);
    QStringList getItemValueList(const DBRecord&);
    DBRecord* getItemByCode(const int);
    DBRecord* getItemByIndexInCurrentGroup(const int);
    bool onManualInputItemValue(const int, const QString&);
    QList<int> getCurrentGroupItemCodes();
    QString getCurrentGroupName();
    void update(const DBRecordList&);
    //static int getBoudrate(const int);
    bool isGroupItem(const DBRecord&);
    void sort();

    int currentGroupCode = 0;

private:
    DBRecordList items;
};

#endif // SETTINGS_H
