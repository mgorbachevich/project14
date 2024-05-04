#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qglobal.h>
#include <QString>

#define APP_VERSION "2.32"

// Debug log:
#define DEBUG_LOG
#define DEBUG_ONTIMER_MESSAGE false
#define DEBUG_MEMORY_MESSAGE false
#define DEBUG_ONTIMER_EQUIPMENT_MESSAGE false
#define DEBUG_WEIGHT_STATUS false

// On Start:
#define CREATE_DEFAULT_DATA_ON_START false
#define CREATE_DEFAULT_IMAGES_ON_START false
#define REMOVE_SETTINGS_FILE_ON_START true
#define REMOVE_DEBUG_LOG_ON_START true
#define REMOVE_PRODUCT_DB_ON_START false
#define REMOVE_LOG_DB_ON_START false
#define REMOVE_TEMP_DB_ON_START true
#define CHECK_AUTHORIZATION true

// Files and Pathes:
#define DEBUG_LOG_NAME "DebugLog.txt"
#define DB_LOG_NAME "Log.db"
#define DB_TEMP_NAME "Temp.db"
#define DB_PRODUCT_NAME "ShtrihScale.db"
#define DOWNLOAD_SUBDIR "Downloads"
#define DUMMY_IMAGE_FILE "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define DEFAULT_EQUIPMENT_DEMO_CONFIG_FILE ":/Text/json_demo_equipment_config.txt"
#define BEEP_SOUND_FILE "qrc:/Sound/KeypressInvalid.mp3"
#define CLICK_SOUND_FILE "qrc:/Sound/KeypressStandard.mp3"

#ifdef Q_OS_ANDROID
#define ANDROID_NATIVE_CLASS_NAME "ru.shtrih_m.shtrihprint6/AndroidNative"
#define EQUIPMENT_CONFIG_FILE "/storage/emulated/0/shtrihm/json_settingsfile.txt"
#define SETTINGS_FILE "/storage/emulated/0/shtrihm/config.txt"
#define USERS_FILE "/storage/emulated/0/shtrihm/users.txt"
#else
#define EQUIPMENT_CONFIG_FILE Tools::dbPath("json_settingsfile.txt")
#define SETTINGS_FILE Tools::dbPath("config.txt")
#define USERS_FILE Tools::dbPath("users.txt")
#endif

// Other:
#define SHOW_PATH_MESSAGE false
#define SERVER_WAIT_FOR_REPLY_MSEC 5000
#define SERVER_WAIT_FOR_REPLY_SLEEP_MSEC 10
#define APP_TIMER_MSEC 10000
#define WAIT_SECRET_MSEC 5000
#define WAIT_SOUND_MSEC 500
#define WAIT_DRAWING_MSEC 150
#define WAIT_NET_ACTION_SEC 3
#define EOL "\r\n"
#define SHOWCASE_ROW_IMAGES 5
#define PRODUCT_STRING_DELIMETER "   "
#define MAX_REMOVE_OLD_LOG_RECORDS_COUNTER 5
#define PRICE_MAX_CHARS 6
#define AMOUNT_MAX_CHARS 8
#define NO_DATA "-----"
//#define DB_EMULATION
#define DEFAULT_ADMIN_NAME "АДМИНИСТРАТОР"
#define DEFAULT_ADMIN_CODE 0

// UI:
#define DEFAULT_SCREEN_WIDTH 568
#define DEFAULT_SCREEN_HEIGHT 320
#define DEFAULT_SPACER 8
#define DEFAULT_WEIGHT_VALUE_FONT_SIZE 70
#define DEFAULT_WEIGHT_TITLE_FONT_SIZE 10
#define DEFAULT_NORMAL_FONT_SIZE 13
#define DEFAULT_LARGE_FONT_SIZE 18
#define DEFAULT_BUTTON_SIZE 40
#define DEFAULT_FLAG_SIZE 20
#define DEFAULT_EDIT_WIDTH 200

#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>
#define StringPair QPair<QString, QString>

enum ConfirmSelector
{
    ConfirmSelector_None = 0,
    ConfirmSelector_Authorization,
    ConfirmSelector_ClearLog,
    ConfirmSelector_SetSystemDateTime,
    ConfirmSelector_ReplaceUser,
    ConfirmSelector_DeleteUser
};

enum LogType
{
    LogType_None = 0,
    LogType_Error = 1,
    LogType_Warning = 2,
    LogType_Info = 3,
    LogType_Debug = 4,
    LogType_Transaction = 5
};

enum LogSource
{
    LogSource_Print = 0,
    LogSource_Admin = 1,
    LogSource_User = 2,
    LogSource_DB = 3,
    LogSource_Weight = 4,
};

enum LogError
{
    LogError_WrongRequest = 1101,
    LogError_WrongRecord = 1102,
    LogError_UnknownTable = 1103,
    LogError_RecordNotFound = 1104,
};

enum ProductType
{
    ProductType_Weight = 0,
    ProductType_Piece = 1,
    ProductType_Group = 2
};

enum ProductReset
{
    ProductReset_None = 0, // нет
    ProductReset_Print = 1, // после печати
    ProductReset_Time = 2, // через заданное время бездействия (секунды)
};

enum ProductPriceBase
{
    ProductPriceBase_Kg = 0,
    ProductPriceBase_Piece = 1,
    ProductPriceBase_100g = 2
};

enum UserRole
{
    UserRole_Admin = 0,
    UserRole_Operator = 1,
};

enum EquipmentDevice
{
    EquipmentDevice_Weight = 0,
    EquipmentDevice_Print = 1,
};

enum ControlParam
{
    ControlParam_None = 0,
    ControlParam_Tare = 1,
    ControlParam_Zero = 2,
    ControlParam_TareValue = 3,
    ControlParam_WeightValue = 4,
    ControlParam_PriceValue = 5,
    ControlParam_AmountValue = 6,
    ControlParam_WeightColor = 7,
    ControlParam_PriceColor = 8,
    ControlParam_AmountColor = 9,
    ControlParam_WeightTitle = 10,
    ControlParam_PriceTitle = 11,
    ControlParam_AmountTitle = 12,
    ControlParam_WeightError = 13,
    ControlParam_WeightFixed = 14,
    ControlParam_PrintError = 15,
    ControlParam_AutoPrint = 16,
    ControlParam_PrinterStatus = 17,
};

enum NetAction
{
    NetAction_Upload,
    NetAction_UploadFinished,
    NetAction_Download,
    NetAction_DownloadFinished,
    NetAction_Delete,
    NetAction_DeleteFinished,
};

enum VirtualKeyboardSet
{
    VirtualKeyboardSet_Latin = 0,
    VirtualKeyboardSet_Cyrillic = 1,
    VirtualKeyboardSet_Numeric = 2,
};

enum Sort
{
    Sort_Code = 0,
    Sort_Name = 1,
};

enum EnvironmentType
{
    EnvironmentType_USB = 0,
    EnvironmentType_Bluetooth = 1,
    EnvironmentType_WiFi = 2,
    EnvironmentType_SDCard = 3,
};

enum MemoryType
{
    MemoryType_Max = 0,
    MemoryType_Available = 1,
    MemoryType_Used = 2
};

enum EquipmentMode
{
    EquipmentMode_None = 0,
    EquipmentMode_Ok = 1,
    EquipmentMode_Demo = 2,
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

#endif // CONSTANTS_H
