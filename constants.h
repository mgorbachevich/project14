#ifndef CONSTANTS_H
#define CONSTANTS_H

#define APP_VERSION "1.1"
#define DB_VERSION "1.0"

#define WEIGHT_EMULATION
//#define WEIGHT_0_ALLWAYS
#define DB_EMULATION
#define HTTP_SERVER
#define HTTP_CLIENT
#define HTTP_CLIENT_TEST
//#define CHECK_AUTHORIZATION
#define SAVE_TRANSACTION_ON_PRINT
#define SAVE_LOG_IN_DB

//#define DEBUG_LOG_BACKGROUND_THREADS
//#define DEBUG_LOG_SELECT_RESULT
//#define DEBUG_LOG_SETTING_GROUPS

#define DATA_STORAGE_SUBDIR "ShtrihScale"
#define DUMMY_IMAGE_FILE "../Images/image_dummy.png"
#define DEFAULT_SETTINGS_FILE ":/Text/json_default_settings.txt"
#define DEFAULT_SETTING_GROUPS_FILE ":/Text/json_default_setting_groups.txt"

#define WEIGHT_0 "00.000"
#define PRICE_0 "0000.00"
#define AMOUNT_0 "000000.00"

#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>

enum DialogSelector
{
    None = 0,
    Authorization,
};

#endif // CONSTANTS_H
