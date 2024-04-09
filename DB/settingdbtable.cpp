#include "settingdbtable.h"
#include "tools.h"
#include "jsonparser.h"
#include "database.h"

SettingDBTable::SettingDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ SettingDBTable::SettingDBTable");

    addColumn("Код",             "code",       "INT PRIMARY KEY");
    addColumn("Тип",             "type",       "INT");
    addColumn("Код группы",      "group_code", "INT");
    addColumn("Наименование",    "name",       "TEXT");
    addColumn("Значение",        "value",      "TEXT");
    addColumn("Список значений", "value_list", "TEXT");
}
/*
const DBRecord SettingDBTable::checkRecord(const DBRecord& record)
{
    // qDebug() << "@@@@@ SettingDBTable::checkRecord: table =" << name;
    bool ok = record.count() >= columnCount();
    if(ok)
    {
        int code = Tools::stringToInt(record.at(Columns::Code).toString());
        int value = Tools::stringToInt(record.at(Columns::Value).toString());
        switch (code)
        {
        case 0:
            ok = false;
            break;
        case SettingCode_ScalesNumber:
            ok = value > 0 && value <= 999999;
            break;
        case SettingCode_PointPosition:
        case SettingCode_PointPositionQuantity:
            ok = value >= 0 && value <= 3;
            break;
        case SettingCode_ProductReset:
            ok = value >= ProductReset_None && value <= ProductReset_Time;
            break;
        case SettingCode_ProductResetTime:
            ok = value >= 0;
            break;
        case SettingCode_SearchType:
            ok = value >= 0 && value <= 1;
            break;
        case SettingCode_Logging:
            ok = value >= 0 && value <= 5;
            break;
        case SettingCode_SearchCodeSymbols:
        case SettingCode_SearchBarcodeSymbols:
            ok = value >= 0 && value <= 9;
            break;
        default: break; // Без проверки
        }
    }
    DBRecord result;
    if(ok)
        result.append(record);
    else
        qDebug() << "@@@@@ SettingDBTable::checkRecord ERROR";
    return result;
}
*/
const DBRecordList SettingDBTable::checkList(DataBase* db, const DBRecordList& records)
{
    Tools::debugLog("@@@@@ SettingDBTable::checkList");
    DBRecordList result;

    // Проверка допустимых значений:
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord r = checkRecord(records[i]);
        if (!r.isEmpty()) result.append(r);
    }

    // Добавление недостающих значений по умолчанию:
    DBRecordList defaultRecords = JSONParser::parseTable(this, Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    for(int i = 0; i < SettingCode_Max; i++) checkAndSaveDefaultRecord(db, i, defaultRecords, result);
    return result;
}

void SettingDBTable::checkAndSaveDefaultRecord(DataBase* db, const int code, const DBRecordList& defaults, DBRecordList& results)
{
    //qDebug() << "@@@@@ SettingDBTable::checkAndSaveDefaultRecord " << code;
    for (int i = 0; i < results.count(); i++)
    {
        if (results.at(i).at(Columns::Code).toInt() == code) // уже есть такая запись
        {
            //Tools::debugLog(QString("@@@@@ SettingDBTable::checkAndSaveDefaultRecord exists %1").arg(code));
            return;
        }
    }
    for (int i = 0; i < defaults.count(); i++)
    {
        DBRecord ri = defaults.at(i);
        if (ri.at(Columns::Code).toInt() == code)
        {
            if(db->insertSettingsRecord(ri))
            {
                Tools::debugLog(QString("@@@@@ SettingDBTable::checkAndSaveDefaultRecord insert %1").arg(code));
                results.append(ri);
            }
            return;
        }
    }
}


