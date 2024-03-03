#ifndef CONSTANTS_H
#define CONSTANTS_H

#define APP_VERSION "1.88"
//#define RELEASE

#ifdef RELEASE
#define WM_DEMO false
#define PRINTER_DEMO false
#define WAIT_NET_ACTION_MSEC 30000
#define REMOVE_PRODUCT_DB_ON_START false
#define REMOVE_SETTINGS_DB_ON_START false
#define REMOVE_LOG_DB_ON_START false
#define REMOVE_TEMP_DB true
#else
#define WM_DEMO true
#define PRINTER_DEMO true
#define WAIT_NET_ACTION_MSEC 10000
#define REMOVE_PRODUCT_DB_ON_START false
#define REMOVE_SETTINGS_DB_ON_START true
#define REMOVE_LOG_DB_ON_START false
#define REMOVE_TEMP_DB true
#endif

#define SHOW_DB_PATH_MESSAGE true
#define CHECK_AUTHORIZATION true
#define SERVER_WAIT_FOR_REPLY_MSEC 5000
#define SERVER_WAIT_FOR_REPLY_SLEEP_MSEC 10
#define APP_TIMER_MSEC 10000
#define WAIT_SECRET_MSEC 5000
#define WAIT_SOUND_MSEC 500
#define EOL "\r\n"
#define DB_LOG_NAME "Log.db"
#define DB_TEMP_NAME "Temp.db"
#define DB_PRODUCT_NAME "ShtrihScale.db"
#define DB_SETTINGS_NAME "Settings.db"
#define DOWNLOAD_SUBDIR "Downloads"
#define DUMMY_IMAGE_FILE "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define BEEP_SOUND_FILE "qrc:/Sound/KeypressInvalid.mp3"
#define CLICK_SOUND_FILE "qrc:/Sound/KeypressStandard.mp3"
#define EQUIPMENT_CONFIG_FILE "/mnt/sdcard/shtrihm/json_settingsfile.txt"
#define DEFAULT_EQUIPMENT_CONFIG_FILE ":/Text/json_default_equipment_config.txt"
#define WEIGHT_DEMO_URI "demo://COM3?baudrate=115200&timeout=100"
#define PRINTER_DEMO_URI "demo://COM3?baudrate=115200&timeout=100"
#define SHOWCASE_ROW_IMAGES 5
#define PRODUCT_STRING_DELIMETER "   "

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

enum DialogSelector
{
    DialogSelector_None = 0,
    DialogSelector_Authorization,
    DialogSelector_ClearLog,
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

enum SettingType
{
    SettingType_Group = 0,
    SettingType_ReadOnly = 1,
    SettingType_Custom = 2,
    SettingType_InputNumber = 3,
    SettingType_InputText = 4,
    SettingType_IntervalNumber = 5,
    SettingType_List = 6,
};

enum DBSelector
{
    DBSelector_None = 0,
    DBSelector_GetShowcaseProducts,
    DBSelector_GetShowcaseResources,
    DBSelector_GetImageByResourceCode,
    DBSelector_GetMessageByResourceCode,
    DBSelector_GetProductsByGroupCode,
    DBSelector_GetProductsByGroupCodeIncludeGroups,
    DBSelector_GetProductsByFilteredCode,
    DBSelector_GetProductsByFilteredBarcode,
    DBSelector_GetItemsByCodes,
    DBSelector_GetUsers,
    DBSelector_GetAuthorizationUserByName,
    DBSelector_GetSettingsItemByCode,
    DBSelector_UpdateSettings,
    DBSelector_GetLog,
    DBSelector_RefreshCurrentProduct,
    DBSelector_ReplaceSettingsItem,
    DBSelector_ChangeSettings,
    DBSelector_SetProductByInputCode,
    DBSelector_GetProductsByInputCode,
    DBSelector_GetProductByInputCode,
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
    SettingCode_PointPositionQuantity = 43, // todo
    SettingCode_SearchCodeSymbols = 44,
    SettingCode_SearchBarcodeSymbols = 45,
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
    SettingCode_TCPPort = 1003,
    SettingCode_SearchType = 1004,
    SettingCode_Manufacturer = 1005, // todo
    //SettingCode_WMAddress = 1006,
    //SettingCode_WMBaudrate = 1007,
    //SettingCode_WMTimeout = 1008,
    SettingCode_License = 1009, // todo
    //SettingCode_PrinterAddress = 1010,
    //SettingCode_PrinterBaudrate = 1011,
    //SettingCode_PrinterTimeout = 1012,
    SettingCode_Version = 1013, // todo
    SettingCode_EquipmentVersion = 1014, // todo
    SettingCode_Help = 1015, // todo
    SettingCode_Verification = 1016, // todo
    SettingCode_ReportsDuration = 1017, // todo
    SettingCode_Brightness = 1018, // todo
    SettingCode_SoundVolume = 1019, // todo
    SettingCode_TimeFormat = 1020, // todo
    SettingCode_Time = 1021, // todo
    SettingCode_DateFormat = 1022, // todo
    SettingCode_Date = 1023, // todo
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
    /*
    SettingCode_Power = 17,
    SettingCode_Cursor = 10,
    SettingCode_Language = 1001,
    SettingCode_Currency = 1002,
    SettingCode_SearchCode2Symbols = 46,
    SettingCode_PrintTitle = 201,
      */
    SettingCode_Max = 9999, // Max value
};

enum Sort
{
    Sort_Code = 0,
    Sort_Name = 1,
};

#endif // CONSTANTS_H
