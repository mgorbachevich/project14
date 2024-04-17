#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qglobal.h>

#define APP_VERSION "2.22"

#define DEBUG_LOG
#define CREATE_DEFAULT_DATA_ON_START true
#define REMOVE_SETTINGS_DB_ON_START false
#define REMOVE_DEBUG_LOG_ON_START true
#define REMOVE_PRODUCT_DB_ON_START false
#define REMOVE_LOG_DB_ON_START false
#define REMOVE_TEMP_DB true
#define CHECK_AUTHORIZATION true
#define SHOW_PATH_MESSAGE false
#define DEBUG_ONTIMER_MESSAGE true
#define DEBUG_MEMORY_MESSAGE true
#define DEBUG_ONTIMER_EQUIPMENT_MESSAGE false
#define SERVER_WAIT_FOR_REPLY_MSEC 5000
#define SERVER_WAIT_FOR_REPLY_SLEEP_MSEC 10
#define APP_TIMER_MSEC 10000
#define WAIT_SECRET_MSEC 5000
#define WAIT_SOUND_MSEC 500
#define WAIT_DRAWING_MSEC 150
#define WAIT_NET_ACTION_MSEC 30000
#define EOL "\r\n"
#define DEBUG_LOG_NAME "DebugLog.txt"
#define DB_LOG_NAME "Log.db"
#define DB_TEMP_NAME "Temp.db"
#define DB_PRODUCT_NAME "ShtrihScale.db"
#define DB_SETTINGS_NAME "Settings.db"
#define DOWNLOAD_SUBDIR "Downloads"
#define DUMMY_IMAGE_FILE "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define BEEP_SOUND_FILE "qrc:/Sound/KeypressInvalid.mp3"
#define CLICK_SOUND_FILE "qrc:/Sound/KeypressStandard.mp3"
#define DEFAULT_EQUIPMENT_CONFIG_FILE ":/Text/json_default_equipment_config.txt"
#define WEIGHT_DEMO_URI "demo://COM3?baudrate=115200&timeout=100"
#define PRINTER_DEMO_URI "demo://COM3?baudrate=115200&timeout=100"
#define EQUIPMENT_OFF_URI "OFF"
#define SHOWCASE_ROW_IMAGES 5
#define PRODUCT_STRING_DELIMETER "   "
#define MAX_REMOVE_OLD_LOG_RECORDS_COUNTER 5
//#define DB_EMULATION

#ifdef Q_OS_ANDROID
//#define ANDROID_EQUIPMENT_CONFIG_FILE "/mnt/sdcard/shtrihm/json_settingsfile.txt"
#define ANDROID_EQUIPMENT_CONFIG_FILE "/storage/emulated/0/shtrihm/json_settingsfile.txt"
#define ANDROID_NATIVE_CLASS_NAME "ru.shtrih_m.shtrihprint6/AndroidNative"
#endif

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

#endif // CONSTANTS_H
