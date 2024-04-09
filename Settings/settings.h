#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "constants.h"

typedef struct {
    QString wmUri;
    QString printerUri;
} EquipmentUris;

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
    bool onInputValue(const int, const QString&);
    bool checkValue(const DBRecord&, const QString&);
    QList<int> getCurrentGroupCodes();
    QString getCurrentGroupName();
    void update(const DBRecordList&);
    bool isGroup(const DBRecord&);
    void sort();
    int nativeSettings(const int);
    EquipmentUris parseEquipmentConfig(const QString&);

    int currentGroupCode = 0;
    QString message;

private:
    DBRecordList items;
};

#endif // SETTINGS_H
