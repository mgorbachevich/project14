#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qglobal.h>
#include <QString>

#define APP_VERSION "2.53"

#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>

// Debug and Log:
//#define DEBUG_LOG_FILE
#define DEBUG_ONTIMER_MESSAGE false
#define DEBUG_MEMORY_MESSAGE false
#define DEBUG_ONTIMER_EQUIPMENT_MESSAGE false
#define DEBUG_WEIGHT_STATUS false
#define DB_PATH_MESSAGE false
#define WRITE_CONFIG_FILE_MESSAGE false
#define DEBUG_INSERT_DB_DELAY_MSEC 0

// On Start:
#define REMOVE_SETTINGS_FILE_ON_START false
#define CREATE_DEFAULT_DATA_ON_START false
#define CREATE_DEFAULT_IMAGES_ON_START false
#define REMOVE_DEBUG_LOG_ON_START true
#define REMOVE_PRODUCT_DB_ON_START false
#define REMOVE_LOG_DB_ON_START true
#define CHECK_AUTHORIZATION true

// Files and Pathes:
#define DEBUG_LOG_NAME "DebugLog.txt"
#define DB_LOG_NAME "Log.db"
#define DB_PRODUCT_NAME "ShtrihScale.db"
#define DB_PRODUCT_COPY_NAME "ShtrihScale_copy.db"
#define DOWNLOAD_SUBDIR "Downloads"
#define DUMMY_IMAGE_FILE "../Images/dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define DEFAULT_EQUIPMENT_DEMO_CONFIG_FILE ":/Text/json_demo_equipment_config.txt"
#define BEEP_SOUND_FILE "qrc:/Sound/KeypressInvalid.mp3"
#define CLICK_SOUND_FILE "qrc:/Sound/KeypressStandard.mp3"

#ifdef Q_OS_ANDROID
#define ANDROID_NATIVE_CLASS_NAME "ru.shtrih_m.shtrihprint6/AndroidNative"
#define EQUIPMENT_CONFIG_FILE "/storage/emulated/0/shtrihm/json_settingsfile.txt"
#define SETTINGS_FILE "/storage/emulated/0/shtrihm/config.txt"
#define USERS_FILE "/storage/emulated/0/shtrihm/users.txt"
#define SCALE_CONFIG_FILE "/storage/emulated/0/shtrihm/scale_config.txt"
#else
#define EQUIPMENT_CONFIG_FILE Tools::dbPath("json_settingsfile.txt")
#define SETTINGS_FILE Tools::dbPath("config.txt")
#define USERS_FILE Tools::dbPath("users.txt")
#define SCALE_CONFIG_FILE Tools::dbPath("scale_config.txt")
#endif

// DateTime
#define DATE_FORMAT "dd.MM.yyyy"
#define TIME_FORMAT "HH:mm:ss"
#define TIME_MSEC_FORMAT "hh:mm:ss.zzz"
#define DATE_TIME_FORMAT "dd.MM.yyyy HH:mm:ss"

// UI
#define PRICE_MAX_CHARS 6
#define AMOUNT_MAX_CHARS 8
#define NO_DATA "-----"

// Other:
#define APP_TIMER_MSEC 10000
#define WAIT_SECRET_MSEC 5000
#define WAIT_SOUND_MSEC 500
#define WAIT_DRAWING_MSEC 150
#define WAIT_NET_COMMAND_MSEC 60000
#define SQL_EXECUTION_WAIT_MSEC 30000
#define SQL_EXECUTION_SLEEP_MSEC 50
#define EOL "\r\n"
#define MAX_REMOVE_OLD_LOG_RECORDS_COUNTER 5
//#define DB_EMULATION
#define DEFAULT_ADMIN_NAME "АДМИНИСТРАТОР"
#define DEFAULT_ADMIN_CODE 0
#define DEFAULT_FACTORY_SETTINGS_PASSWORDS true
#define EQUIPMENT_POLLING_INTERVAL 200
#define BACKGROUND_DOWNLOADING false

#define DBTABLENAME_SHOWCASE "showcase"
#define DBTABLENAME_PRODUCTS "products"
#define DBTABLENAME_LABELFORMATS "labels"
#define DBTABLENAME_MESSAGES "messages"
#define DBTABLENAME_MESSAGEFILES "messagefiles"
#define DBTABLENAME_PICTURES "pictures"
#define DBTABLENAME_MOVIES "movies"
#define DBTABLENAME_SOUNDS "sounds"
#define DBTABLENAME_LOG "log"
#define DBTABLENAME_TRANSACTIONS "transactions"
#define DBTABLENAME_SETTINGS "settings"
#define DBTABLENAME_USERS "users"

enum MainPageIndex
{
    MainPageIndex_Authorization = -1,
    MainPageIndex_Showcase = 0,
    MainPageIndex_Search = 1
};

enum ConfirmSelector
{
    ConfirmSelector_None = 0,
    ConfirmSelector_Authorization,
    ConfirmSelector_ClearLog,
    ConfirmSelector_ReplaceUser,
    ConfirmSelector_DeleteUser,
    ConfirmSelector_AddToShowcase,
    ConfirmSelector_RemoveFromShowcase,
    ConfirmSelector_SetDateTime,
    ConfirmSelector_SetVerificationDate
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

enum WMError
{
    WMError_None = 0,
    WMError_AutoZero = 5003, // ошибка автонуля при включении
    WMError_Overload = 5004, // перегрузка по весу
    WMError_Mesure = 5005, // ошибка при получении измерения (нет градуировки весов или она не правильная)
    WMError_Underload = 5006, // весы недогружены
    WMError_NoReply = 5007, // ошибка: нет ответа от АЦП
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
    ControlParam_DateTime = 18,
    ControlParam_ProductImage = 19,
    ControlParam_SearchTitle = 20,
    ControlParam_AuthorizationTitle1 = 21,
    ControlParam_AuthorizationTitle2 = 22,
    ControlParam_AuthorizationTitle3 = 23,
};

enum NetCommand
{
    NetCommand_None = 0,
    NetCommand_Message,
    NetCommand_StartLoad,
    NetCommand_StopLoad,
    NetCommand_Progress
};

enum VirtualKeyboardSet
{
    VirtualKeyboardSet_Latin = 0,
    VirtualKeyboardSet_Cyrillic = 1,
    VirtualKeyboardSet_Numeric = 2,
};

enum ShowcaseSort
{
    ShowcaseSort_Code = 0,
    ShowcaseSort_Code2 = 1,
    ShowcaseSort_Name = 2,
};

enum SearchFilterIndex
{
    SearchFilterIndex_Code = 0,
    SearchFilterIndex_Code2 = 1,
    SearchFilterIndex_Barcode = 2,
    SearchFilterIndex_Name = 3,
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

enum ShowcaseProductText
{
    ShowcaseProductText_None = 0,
    ShowcaseProductText_Code = 1,
    ShowcaseProductText_Code2 = 2,
    ShowcaseProductText_Sort = 3,
    ShowcaseProductText_Name = 4,
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
    SettingCode_ScalesName = 2,
    SettingCode_ProductReset = 7,
    SettingCode_ProductResetTime = 8,
    SettingCode_PointPosition = 12,
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
    SettingCode_SerialScalesNumber = 1010,
    SettingCode_VerificationName = 1011,
    SettingCode_VerificationDate = 1012,
    SettingCode_Version = 1013,
    SettingCode_EquipmentVersion = 1014,
    SettingCode_Help = 1015,
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
    SettingCode_FactorySettings = 9000,
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
