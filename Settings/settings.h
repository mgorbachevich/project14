#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVariantList>
#include "constants.h"

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
};

enum SettingCode // Должны совпадать со значениями в файлах json_default_settings.txt!
{
    SettingCode_None = 0,
    SettingCode_ScalesNumber = 1,
    SettingCode_ScalesName = 2,
    SettingCode_Blocking = 3,
    SettingCode_ProductReset = 7,
    SettingCode_ProductResetTime = 8,
    SettingCode_PointPosition = 12,
    SettingCode_PrinterAutoFeed = 42,
    SettingCode_CharNumberPieces = 43,
    SettingCode_SearchCodeSymbols = 44,
    SettingCode_SearchBarcodeSymbols = 45,
    SettingCode_SearchNameSymbols = 46, // todo
    SettingCode_SearchEquality = 47, // todo
    SettingCode_ShopName = 202,
    SettingCode_PrintAutoWeight = 203,
    SettingCode_PrintPaper = 208,
    SettingCode_PrintAuto = 210,
    SettingCode_PrinterBrightness = 211,
    SettingCode_PrintLabelFormat = 213, // todo
    SettingCode_PrintAutoPcs = 218,
    SettingCode_Logging = 606,
    SettingCode_LogDuration = 607,
    SettingCode_Currency = 1002,
    SettingCode_TCPPort = 1003,
    SettingCode_SearchType = 1004,
    SettingCode_Manufacturer = 1005, // todo
    SettingCode_SetZeroTries = 1006,
    SettingCode_GoToDescription = 1007,
    SettingCode_Level = 1008,
    SettingCode_License = 1009, // todo
    SettingCode_SerialScalesNumber = 1010,
    SettingCode_VerificationName = 1011, // todo
    SettingCode_VerificationDate = 1012, // todo
    SettingCode_Version = 1013, // todo
    SettingCode_EquipmentVersion = 1014, // todo
    SettingCode_Help = 1015, // todo
    // = 1016
    SettingCode_ReportsDuration = 1017, // todo
    //SettingCode_Brightness = 1018, // todo
    SettingCode_KeyboardSoundVolume = 1019,
    SettingCode_DateTime = 1020, // todo
    SettingCode_PrintOffset = 1024, // todo
    SettingCode_PrintLabelSensor = 1025, // todo
    SettingCode_PrintLabelHighlight = 1026, // todo
    SettingCode_PrintLabelRotation = 1027, // todo
    SettingCode_PrintLabelBarcodeWeight = 1028, // todo
    SettingCode_PrintLabelBarcodePiece = 1029, // todo
    SettingCode_Ethernet = 1030,
    SettingCode_WiFi = 1031,
    SettingCode_ClearLog = 1032,
    SettingCode_ClearReports = 1033, // todo
    SettingCode_HorizontalCalibration = 1034, // todo
    SettingCode_ResetAll = 1035, // todo
    SettingCode_UpdateCore = 1036, // todo
    SettingCode_UpdateApp = 1037, // todo
    SettingCode_UpdateDemon = 1038, // todo
    SettingCode_WMReset = 1039, // todo
    SettingCode_WMCalibration = 1040, // todo
    SettingCode_PrinterReset = 1041, // todo
    SettingCode_PrinterCalibration = 1042, // todo
    SettingCode_WMInfo = 1043, // todo
    SettingCode_PrinterInfo = 1044, // todo
    SettingCode_Equipment = 1045,
    SettingCode_SystemSettings = 1046,
    SettingCode_Update = 1047,
    SettingCode_PrintLabelPrefixWeight = 1048,
    SettingCode_PrintLabelPrefixPiece = 1049,
    /*
    SettingCode_Power = 17,
    SettingCode_Cursor = 10,
    SettingCode_Language = 1001,
    SettingCode_SearchCode2Symbols = 46,
    SettingCode_PrintTitle = 201,
      */
    SettingCode_Max = 9999, // Max value
};

typedef struct
{
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
    QString getAndClearMessage();
    void onShow();

    int currentGroupCode = 0;

private:
    DBRecordList items;
    QString message;
};

#endif // SETTINGS_H
