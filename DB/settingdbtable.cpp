#include "settingdbtable.h"
#include "tools.h"
#include "jsonparser.h"
#include "database.h"

SettingDBTable::SettingDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    qDebug() << "@@@@@ SettingDBTable::SettingDBTable";
    addColumn("Код",          "code",       "INT PRIMARY KEY");
    addColumn("Тип",          "type",       "INT");
    addColumn("Код группы",   "group_code", "INT");
    addColumn("Наименование", "name",       "TEXT");
    addColumn("Значение",     "value",      "TEXT");
}

const DBRecord SettingDBTable::checkRecord(const DBRecord& record)
{
    qDebug() << "@@@@@ SettingDBTable::checkRecord: table =" << name;
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
        case Settings::SettingCode_ScalesNumber:
            ok = value > 0 && value <= 999999;
            break;
        case Settings::SettingCode_PointPosition:
            ok = value >= 0 && value <= 3;
            break;
        case Settings::SettingCode_ProductReset:
            ok = value >= Settings::ProductReset_None && value <= Settings::ProductReset_Time;
            break;
        case Settings::SettingCode_ProductResetTime:
            ok = value >= 0;
            break;
        case Settings::SettingCode_SearchType:
            ok = value >= 0 && value <= 1;
            break;
        case Settings::SettingCode_Logging:
            ok = value >= 0 && value <= 5;
            break;
        case Settings::SettingCode_SearchCodeSymbols:
        case Settings::SettingCode_SearchBarcodeSymbols:
            ok = value >= 0 && value <= 9;
            break;
        case Settings::SettingCode_WMBaudrate:
            ok = value >= 0 && value <= 6;
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

const DBRecordList SettingDBTable::checkList(DataBase* db, const DBRecordList& records)
{
    qDebug() << "@@@@@ SettingDBTable::checkList";
    DBRecordList result;

    // Проверка допустимых значений:
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord r = checkRecord(records[i]);
        if (!r.isEmpty()) result.append(r);
    }

    // Добавление недостающих значений по умолчанию:
    DBRecordList defaultRecords = JSONParser::parseTable(this, Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    for(int i = 0; i < Settings::SettingCode_Max; i++) checkAndSaveDefaultRecord(db, i, defaultRecords, result);
    return result;
}

void SettingDBTable::checkAndSaveDefaultRecord(DataBase* db, const int code, const DBRecordList& defaults, DBRecordList& results)
{
    for (int i = 0; i < results.count(); i++)
    {
        if (results.at(i).at(Columns::Code).toInt() == code) return; // уже есть такая запись
    }
    for (int i = 0; i < defaults.count(); i++)
    {
        DBRecord ri = defaults.at(i);
        if (ri.at(Columns::Code).toInt() == code)
        {
            if(db->insertRecord(this, ri))
            {
                qDebug() << "@@@@@ SettingDBTable::checkAndSaveDefaultRecord " << code;
                results.append(ri);
            }
            return;
        }
    }
}


