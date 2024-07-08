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
    bool getBoolValue(const SettingCode code) { return getIntValue(code, true) != 0; }
    DBRecord* getByIndexInCurrentGroup(const int);
    static int getCode(const DBRecord& r) { return r[SettingField_Code].toInt(); }
    static QString getComment(const DBRecord& r) { return r[SettingField_Comment].toString(); }
    QList<int> getCurrentGroupCodes();
    int getCurrentGroupCode() { return currentGroupCode; }
    QString getCurrentGroupName();
    int getIntValue(const SettingCode, const bool listIndex = false);
    int getIntValue(const DBRecord&, const bool listIndex = false);
    static QString getName(const DBRecord& r) { return r[SettingField_Name].toString(); }
    QString getName(const SettingCode code) { return getName(*getByCode(code)); }
    QString getStringValue(const DBRecord&);
    QString getStringValue(const SettingCode);
    static int getType(const DBRecord& r) { return r[SettingField_Type].toInt(); }
    static QStringList getValueList(const DBRecord& r) { return r[SettingField_ValueList].toString().split(','); }
    static bool isGroup(const DBRecord& r);
    void nativeSettings(const int);
    bool setValue(const int, const QString&);
    void update(const int);
    void clear() {}
    bool read();
    bool write();
    bool readConfig() { return scaleConfig->read(); }
    bool writeConfig();
    QString getConfigValue(const ScaleConfigField f) { return scaleConfig->get(f).toString(); }
    void setConfigValue(const ScaleConfigField f, const QString& v) { scaleConfig->set(f, v); }
    void setConfigDateTime(const ScaleConfigField);

protected:
    void sort();
    bool checkValue(const DBRecord&, const QString&);
    bool parseDefault();
    void appendItemToJson(DBRecord&, QJsonArray&);

    int currentGroupCode = 0;
    ScaleConfig* scaleConfig;
};

#endif // SETTINGS_H
