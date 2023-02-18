#ifndef SETTINGDBTABLE_H
#define SETTINGDBTABLE_H

#include "dbtable.h"

class SettingDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0, // enum
        Name,
        Value,
        COLUMN_COUNT
    };

    enum SettingCode
    {
        SettingCode_None = 0,
        SettingCode_ScalesNumber = 1,
        SettingCode_ScalesName = 2,
        SettingCode_ShopName = 202,
        SettingCode_TCPPort = 1003,
        /*
        SettingCode_Power = 17,
        SettingCode_Blocking = 3,
        SettingCode_Cursor = 10,
        SettingCode_Language = 1001,
        SettingCode_Currency = 1002,
        SettingCode_PointPosition = 12,
        SettingCode_ProductReset = 7,
        SettingCode_ProductResetTime = 8,
        SettingCode_SearchType = 1004,
        SettingCode_SearchCodeSymbols = 44,
        SettingCode_SearchBarcodeSymbols = 45,
        SettingCode_SearchCode2Symbols = 46,
        SettingCode_SearchEquality = 47,
        SettingCode_PrinterAutoFeed = 42,
        SettingCode_PrinterBrightness = 211,
        SettingCode_PrintTitle = 201,
        SettingCode_PrintPaper = 208,
        SettingCode_PrintAuto = 210,
        SettingCode_PrintAutoWeight = 203,
        SettingCode_PrintAutoPcs = 218,
        SettingCode_PrintLabelFormat = 213,
        SettingCode_Logging = 606,
        SettingCode_LogDuration = 607,
        */
    };

    SettingDBTable(const QString&, QObject*);
    int columnCount() { return Columns::COLUMN_COUNT; }
    const DBRecordList checkAll(const DBRecordList&);

private:
    void checkDefault(const SettingCode, const QList<int>&, const DBRecordList&, DBRecordList&);

};

#endif // SETTINGDBTABLE_H
