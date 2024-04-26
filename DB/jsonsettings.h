#ifndef JSONSETTINGS_H
#define JSONSETTINGS_H

#include <QVariantList>
#include "jsonfile.h"
#include "constants.h"

#ifdef JSON_SETTINGS

enum SettingField
{
    SettingField_Code = 0,
    SettingField_Type = 1,
    SettingField_GroupCode = 2,
    SettingField_Name = 3,
    SettingField_Value = 4,
    SettingField_ValueList = 5
};

class Settings : public JsonFile
{
public:
    Settings(QObject*);
    bool getBoolValue(const SettingCode);
    DBRecord* getByCode(const int);
    DBRecord* getByIndexInCurrentGroup(const int);
    int getCode(const DBRecord&);
    QList<int> getCurrentGroupCodes();
    QString getCurrentGroupName();
    int getIntValue(const SettingCode, const bool listIndex = false);
    int getIntValue(const DBRecord&, const bool listIndex = false);
    QString getName(const DBRecord&);
    QString getStringValue(const DBRecord&);
    QString getStringValue(const SettingCode);
    int getType(const DBRecord&);
    QStringList getValueList(const DBRecord&);
    bool isGroup(const DBRecord&);
    int nativeSettings(const int);
    bool onInputValue(const int, const QString&);
    void onShow();
    static EquipmentUris parseEquipmentConfig(const QString&);
    bool read();
    void sort();

    int currentGroupCode = 0;

protected:
    bool checkValue(const DBRecord&, const QString&);
    void checkDefaultRecord(const int, DBRecordList&);
};

#endif // JSON_SETTINGS
#endif // JSONSETTINGS_H
