#ifndef CONSTANTS_H
#define CONSTANTS_H

#define APP_VERSION "1.25"

#define DB_EMULATION
#define CHECK_AUTHORIZATION

#define DATA_STORAGE_SUBDIR "ShtrihScale"
#define DB_FILENAME "ShtrihScale.db"
#define DB_HOSTNAME "ShtrihScaleDataBase"
#define DUMMY_IMAGE_FILE "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define DEFAULT_SETTING_GROUPS_FILE ":/Text/json_default_setting_groups.txt"
#define SERVER_WAIT_FOR_REPLY_MSEC 20000
#define SERVER_WAIT_FOR_REPLY_SLEEP_MSEC 10

#define NetReply QPair<qint64, QString>
#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>

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
    LogSource_Print,
    LogSource_Admin,
    LogSource_User,
    LogSource_DB,
    LogSource_Weight,
};

enum LogError
{
    LogError_WrongRequest = 1101,
    LogError_WrongRecord = 1102,
    LogError_UnknownTable = 1103,
    LogError_RecordNotFound = 1104,
};

enum EquipmentParam
{
    EquipmentParam_None = 0,
    EquipmentParam_TareFlag = 1,
    EquipmentParam_ZeroFlag = 2,
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
    EquipmentParam_Error = 13,
    EquipmentParam_WeightFixed = 14,
};

#endif // CONSTANTS_H
