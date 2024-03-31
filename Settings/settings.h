#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "constants.h"

class Settings
{
public:
    Settings() {}
    int getCode(const DBRecord&);
    int getType(const DBRecord&);
    QString getName(const DBRecord&);
    int getIntValue(const SettingCode, const bool listIndex = false);
    int getIntValue(const DBRecord&, const bool listIndex = false);
    bool getBoolValue(const SettingCode);
    QString getStringValue(const SettingCode);
    QString getStringValue(const DBRecord&);
    QStringList getValueList(const DBRecord&);
    DBRecord* getByCode(const int);
    DBRecord* getByIndexInCurrentGroup(const int);
    bool onManualInputValue(const int, const QString&);
    QList<int> getCurrentGroupCodes();
    QString getCurrentGroupName();
    void update(const DBRecordList&);
    bool isGroup(const DBRecord&);
    void sort();
    int nativeSettings(const int);
    QList<QString> parseEquipmentConfig(const QString&);

    int currentGroupCode = 0;

private:
    DBRecordList items;
};

#endif // SETTINGS_H
