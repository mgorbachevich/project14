#ifndef SETTINGS_H
#define SETTINGS_H

#include "jsonarrayfile.h"
#include "constants.h"
#include "scaleconfig.h"

enum SettingField
{
    SettingField_Code = 0,
    SettingField_Type = 1,
    SettingField_GroupCode = 2,
    SettingField_Name = 3,
    SettingField_Value = 4,
    SettingField_ValueList = 5,
    SettingField_Comment = 6
};

class Settings : public JsonArrayFile
{
public:
    Settings(AppManager*);
    void apply();
    bool getBoolValue(const SettingCode code) { return getIntValue(code, true) != 0; }
    DBRecord* getByIndexInCurrentGroup(const int);
    int getCode(const DBRecord& r) { return r[SettingField_Code].toInt(); }
    QString getComment(const DBRecord& r) { return r[SettingField_Comment].toString(); }
    QList<int> getCurrentGroupCodes();
    int getCurrentGroupCode() { return currentGroupCode; }
    QString getCurrentGroupName();
    int getIntValue(const SettingCode, const bool listIndex = false);
    int getIntValue(const DBRecord&, const bool listIndex = false);
    QString getName(const DBRecord& r) { return r[SettingField_Name].toString(); }
    QString getName(const SettingCode code) { return getName(*getByCode(code)); }
    QString getStringValue(const DBRecord&);
    QString getStringValue(const SettingCode);
    int getType(const DBRecord& r) { return r[SettingField_Type].toInt(); }
    QStringList getValueList(const DBRecord& r) { return r[SettingField_ValueList].toString().split(','); }
    bool isGroup(const DBRecord& r);
    void nativeSettings(const int);
    bool setValue(const int, const QString&);
    void update(const int);
    void clear() {}
    bool read();
    bool write();
    QString getScaleConfigValue(const ScaleConfigField field) { return scaleConfig->get(field).toString(); }

protected:
    void sort();
    bool checkValue(const DBRecord&, const QString&);
    void checkDefaultRecord(const int, DBRecordList&);
    bool parseDefault();

    int currentGroupCode = 0;
    ScaleConfig* scaleConfig;
};

#endif // SETTINGS_H
