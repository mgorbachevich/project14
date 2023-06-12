#ifndef CONSTANTS_H
#define CONSTANTS_H

#define APP_VERSION "1.23"
#define DB_VERSION "1.0"

#define DB_EMULATION
#define CHECK_AUTHORIZATION
#define SAVE_TRANSACTION_ON_PRINT
#define SAVE_LOG_IN_DB

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

#endif // CONSTANTS_H
