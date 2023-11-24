#ifndef SETTINGDBTABLE_H
#define SETTINGDBTABLE_H

#include "dbtable.h"

class SettingDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0, // enum SettingCode
        Type, // enum SettingType
        GroupCode, // enum SettingCode
        Name,
        Value,
        COLUMN_COUNT
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
        SettingCode_SearchCodeSymbols = 44,
        SettingCode_SearchBarcodeSymbols = 45,
        SettingCode_ShopName = 202,
        SettingCode_PrintAutoWeight = 203,
        SettingCode_PrintPaper = 208,
        SettingCode_PrintAuto = 210,
        SettingCode_PrintLabelFormat = 213,
        SettingCode_PrintAutoPcs = 218,
        SettingCode_Logging = 606,
        SettingCode_LogDuration = 607,
        SettingCode_TCPPort = 1003,
        SettingCode_SearchType = 1004,
        //SettingCode_WMDemo = 1005,
        SettingCode_WMAddress = 1006,
        SettingCode_WMBaudrate = 1007,
        SettingCode_WMTimeout = 1008,
        //SettingCode_PrinterDemo = 1009,
        SettingCode_PrinterAddress = 1010,
        SettingCode_PrinterBaudrate = 1011,
        SettingCode_PrinterTimeout = 1012,
        /*
        SettingCode_Power = 17,
        SettingCode_Cursor = 10,
        SettingCode_Language = 1001,
        SettingCode_Currency = 1002,
        SettingCode_PointPositionQuantity = 43,
        SettingCode_SearchCode2Symbols = 46,
        SettingCode_SearchEquality = 47,
        SettingCode_PrinterAutoFeed = 42,
        SettingCode_PrinterBrightness = 211,
        SettingCode_PrintTitle = 201,
          */
        SettingCode_Max = 9999, // Max value
    };

    enum SettingType
    {
        SettingType_Item = 0,
        SettingType_Group = 1,
    };

    enum WMBaudrate
    {
        WMBaudrate_2400 = 0,
        WMBaudrate_4800 = 1,
        WMBaudrate_9600 = 2,
        WMBaudrate_19200 = 3,
        WMBaudrate_38400 = 4,
        WMBaudrate_57600 = 5,
        WMBaudrate_115200 = 6,
    };

    enum ProductReset
    {
        ProductReset_None = 0, // нет
        ProductReset_Print = 1, // после печати
        ProductReset_Time = 2, // через заданное время бездействия (секунды)
    };

    SettingDBTable(const QString&, QObject*);
    int columnCount() { return Columns::COLUMN_COUNT; }
    const DBRecord checkRecord(const DBRecord&);
    const DBRecordList checkList(const DBRecordList&);
    static int getBoudrate(const int);

private:
    void checkDefault(const int, const QList<QVariantList>&, QList<QVariantList>&);
};

#endif // SETTINGDBTABLE_H
