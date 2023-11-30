#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CLEAR_DB_ON_START true
#define WM_DEMO true
#define PRINTER_DEMO true
#define WM_HTTP true
#define PRINTER_HTTP true
#define CHECK_AUTHORIZATION true

#define DOWNLOAD_SUBDIR "Downloads"
#define DB_FILENAME "ShtrihScale.db"
#define DB_HOSTNAME "ShtrihScaleDataBase"
#define DUMMY_IMAGE_FILE_QML_PATH "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define SERVER_WAIT_FOR_REPLY_MSEC 5000
#define SERVER_WAIT_FOR_REPLY_SLEEP_MSEC 10
#define APP_TIMER_MSEC 10000
#define WAIT_SECRET_MSEC 5000
#define EOL "\r\n"

#define DEFAULT_SCREEN_WIDTH 568
#define DEFAULT_SCREEN_HEIGHT 320
#define DEFAULT_SPACER 8
#define DEFAULT_WEIGHT_VALUE_FONT_SIZE 70
#define DEFAULT_WEIGHT_TITLE_FONT_SIZE 10
#define DEFAULT_NORMAL_FONT_SIZE 13
#define DEFAULT_LARGE_FONT_SIZE 18
#define DEFAULT_BUTTON_SIZE 40
#define DEFAULT_EDIT_WIDTH 200

#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>
#define StringPair QPair<QString, QString>

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

enum AutoPrint
{
    AutoPrint_None = 0,
    AutoPrint_Enable = 1,
    AutoPrint_Disable = 2,
};

enum EquipmentParam
{
    EquipmentParam_None = 0,
    EquipmentParam_Tare = 1,
    EquipmentParam_Zero = 2,
    EquipmentParam_TareValue = 3,
    EquipmentParam_WeightValue = 4,
    EquipmentParam_PriceValue = 5,
    EquipmentParam_AmountValue = 6,
    EquipmentParam_WeightColor = 7,
    EquipmentParam_PriceColor = 8,
    EquipmentParam_AmountColor = 9,
    EquipmentParam_WeightTitle = 10,
    EquipmentParam_PriceTitle = 11,
    EquipmentParam_AmountTitle = 12,
    EquipmentParam_WeightError = 13,
    EquipmentParam_WeightFixed = 14,
    EquipmentParam_PrintError = 15,
    EquipmentParam_AutoPrint = 16,
};

enum NetRequest
{
    NetRequest_Get,
    NetRequest_Set,
    NetRequest_Delete,
};

enum VirtualKeyboardSet
{
    VirtualKeyboardSet_Latin = 0,
    VirtualKeyboardSet_Cyrillic = 1,
    VirtualKeyboardSet_Numeric = 2,
};

#endif // CONSTANTS_H
