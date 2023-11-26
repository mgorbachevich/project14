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

    SettingDBTable(const QString&, QObject*);
    int columnCount() { return Columns::COLUMN_COUNT; }
    const DBRecord checkRecord(const DBRecord&);
    const DBRecordList checkList(DataBase*, const DBRecordList&);

private:
    void checkAndSaveDefaultRecord(DataBase*, const int, const DBRecordList&, DBRecordList&);
};

#endif // SETTINGDBTABLE_H
