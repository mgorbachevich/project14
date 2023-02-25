#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WEIGHT_EMULATION
//#define WEIGHT_0_ALLWAYS
#define DB_EMULATION
//#define LOG_BACKGROUND_THREADS
//#define LOG_SELECT_RESULT
#define HTTP_SERVER
#define HTTP_CLIENT
//#define HTTP_CLIENT_TEST
//#define CHECK_AUTHORIZATION
#define SAVE_TRANSACTION_ON_PRINT

#define DATA_STORAGE_SUBDIR "ShtrihScale"
#define WEIGHT_0 "00.000"
#define PRICE_0 "0000.00"
#define AMOUNT_0 "000000.00"
#define DUMMY_IMAGE "../Images/image_dummy.png"

#define DBRecord QVariantList
#define DBRecordList QList<QVariantList>

enum DialogSelector
{
    None = 0,
    Authorization,
};

#endif // CONSTANTS_H
