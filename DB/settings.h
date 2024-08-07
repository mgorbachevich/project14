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

enum SettingType
{
    SettingType_Group = 0,
    SettingType_ReadOnly = 1,
    SettingType_Custom = 2,
    SettingType_InputNumber = 3,
    SettingType_InputText = 4,
    SettingType_IntervalNumber = 5,
    SettingType_List = 6,
    SettingType_Unsed = 7,
    SettingType_GroupWithPassword = 8,
    SettingType_UnsedGroup = 9,
};

enum SettingCode // Должны совпадать со значениями в файлах json_default_settings.txt!
{
    SettingCode_None = 0,
    SettingCode_ScalesNumber = 1,
    SettingCode_Model = 2,
    SettingCode_ProductReset = 7,
    SettingCode_ProductResetTime = 8,
    SettingCode_PointPosition = 12,
    SettingCode_Site = 13,
    SettingCode_Support = 14,
    SettingCode_Phone = 15,
    SettingCode_InfoLicense = 16,
    SettingCode_InfoMODVersion = 17,
    SettingCode_InfoDaemonVersion = 18,
    SettingCode_InfoDBVersion = 19,
    SettingCode_InfoServerVersion = 20,
    SettingCode_InfoLastDownload = 21,
    SettingCode_InfoWiFiSSID = 22,
    SettingCode_InfoIP = 23,
    SettingCode_InfoBluetooth = 24,
    SettingCode_InfoAndroidBuild = 25,
    SettingCode_InfoWMVersion = 26,
    SettingCode_InfoPMVersion = 27,
    SettingCode_InfoNetName = 30,
    SettingCode_InfoVersion = 31,
    SettingCode_InfoModelName = 32,
    SettingCode_InfoSerialNumber = 33,
    SettingCode_PrinterAutoFeed = 42,
    SettingCode_CharNumberPieces = 43,
    SettingCode_SearchCodeSymbols = 44,
    SettingCode_SearchBarcodeSymbols = 45,
    SettingCode_SearchNameSymbols = 46,
    SettingCode_SearchEquality = 47,
    SettingCode_ShopName = 202,
    SettingCode_PrintAutoWeight = 203,
    SettingCode_PrintPaper = 208,
    SettingCode_PrintAuto = 210,
    SettingCode_PrinterBrightness = 211,
    SettingCode_PrintLabelFormat = 213,
    SettingCode_PrintAutoPcs = 218,
    SettingCode_Logging = 606,
    SettingCode_LogDuration = 607,
    SettingCode_Currency = 1002,
    SettingCode_TCPPort = 1003,
    SettingCode_SearchType = 1004,
    SettingCode_Manufacturer = 1005,
    SettingCode_SetZeroTries = 1006,
    SettingCode_GoToDescription = 1007,
    SettingCode_Level = 1008,
    SettingCode_License = 1009,
    SettingCode_SerialNumber = 1010,
    SettingCode_NetName = 1011,
    SettingCode_VerificationDate = 1012,
    SettingCode_ReportsDuration = 1017,
    SettingCode_DateTime = 1020,
    SettingCode_PrintOffset = 1024,
    SettingCode_PrintLabelSensor = 1025,
    SettingCode_PrintLabelHighlight = 1026,
    SettingCode_PrintLabelRotation = 1027,
    SettingCode_PrintLabelBarcodeWeight = 1028,
    SettingCode_PrintLabelBarcodePiece = 1029,
    SettingCode_Ethernet = 1030,
    SettingCode_WiFi = 1031,
    SettingCode_ClearLog = 1032,
    SettingCode_ClearReports = 1033,
    SettingCode_HorizontalCalibration = 1034,
    SettingCode_ResetAll = 1035,
    SettingCode_UpdateCore = 1036,
    SettingCode_UpdateApp = 1037,
    SettingCode_UpdateDemon = 1038,
    SettingCode_WMReset = 1039,
    SettingCode_WMCalibration = 1040,
    SettingCode_PrinterReset = 1041,
    SettingCode_PrinterCalibration = 1042,
    SettingCode_WMInfo = 1043,
    SettingCode_PrinterInfo = 1044,
    SettingCode_Equipment = 1045,
    SettingCode_SystemSettings = 1046,
    SettingCode_Update = 1047,
    SettingCode_PrintLabelPrefixWeight = 1048,
    SettingCode_PrintLabelPrefixPiece = 1049,
    SettingCode_Users = 1050,
    SettingCode_Blocking = 1051,
    SettingCode_KeyboardSoundVolume = 1052,
    SettingCode_SystemSoundVolume = 1053,
    SettingCode_Brightness = 1054,
    SettingCode_ShowcaseProductTopText = 1055,
    SettingCode_ShowcaseProductBottomText = 1056,
    SettingCode_ChangeShowcase = 1057,
    //Группы:
    SettingCode_Group_System = 8100,
    SettingCode_Group_WorkingMode = 8200,
    SettingCode_Group_Showcase = 8201,
    SettingCode_Group_Search = 8300,
    SettingCode_Group_Advertisement = 8300,
    SettingCode_Group_Video = 8500,
    SettingCode_Group_Print = 8600,
    SettingCode_Group_PrintParams = 8601,
    SettingCode_Group_LabelParams = 8602,
    SettingCode_Group_Net = 8700,
    SettingCode_Group_Protocol = 8750,
    SettingCode_Group_LogAndReports = 8800,
    SettingCode_Group_Log = 8801,
    SettingCode_Group_Reports = 8802,
    SettingCode_Group_Info = 8900,
    SettingCode_Group_FactorySettings = 9000,
    SettingCode_Group_Verification = 9001,
     /*
    SettingCode_Power = 17,
    SettingCode_Cursor = 10,
    SettingCode_Language = 1001,
    SettingCode_SearchCode2Symbols = 46,
    SettingCode_PrintTitle = 201,
      */
    SettingCode_Max = 9999, // Max value
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
    void update(const int groupCode) { currentGroupCode = groupCode; }
    void clear() {}
    bool read();
    bool write();
    QString getConfigValue(const ScaleConfigField f) { return scaleConfig->get(f).toString(); }
    void setConfigValue(const ScaleConfigField, const QString&);
    QString aboutInfo();
    QString modelInfo();
    QJsonObject getScaleConfig();

protected:
    void sort() {}
    bool checkValue(const DBRecord&, const QString&);
    void appendItemToJson(DBRecord&, QJsonArray&);
    void setModelValues();
    QString netName();
    int model();
    void fromConfig();
    void toConfig();

    int currentGroupCode = 0;
    ScaleConfig* scaleConfig;
    QList<QPair<QString, QString>> modelNames;
};

#endif // SETTINGS_H
