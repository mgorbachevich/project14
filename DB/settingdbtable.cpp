#include "settingdbtable.h"
#include "tools.h"
#include "jsonparser.h"

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
        case SettingCode_ScalesNumber:
            ok = value > 0 && value <= 999999;
            break;
        case SettingCode_PointPosition:
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
        case SettingCode_WMBaudrate:
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

const DBRecordList SettingDBTable::checkList(const DBRecordList& records)
{
    qDebug() << "@@@@@ SettingDBTable::checkList";
    DBRecordList result;

    // Проверка допустимых значений:
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord r = checkRecord(records[i]);
        if (!r.isEmpty())
            result.append(r);
    }

    // Добавление недостающих значений по умолчанию:
    DBRecordList defaultRecords = JSONParser::parseTable(this, Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    for(int i = 0; i < SettingCode_Max; i++) checkDefault(i, defaultRecords, result);
    return result;
}

int SettingDBTable::getBoudrate(const int code)
{
    switch (code)
    {
    case SettingDBTable::WMBaudrate_2400:   return 2400;
    case SettingDBTable::WMBaudrate_4800:   return 4800;
    case SettingDBTable::WMBaudrate_9600:   return 9600;
    case SettingDBTable::WMBaudrate_19200:  return 19200;
    case SettingDBTable::WMBaudrate_38400:  return 38400;
    case SettingDBTable::WMBaudrate_57600:  return 57600;
    case SettingDBTable::WMBaudrate_115200: return 115200;
    }
    return 9600;
}

void SettingDBTable::checkDefault(const int code, const DBRecordList& defaultRecords, DBRecordList& resultRecords)
{
    for (int i = 0; i < resultRecords.count(); i++)
    {
        if (resultRecords.at(i).at(Columns::Code).toInt() == code) return; // уже есть такая запись
    }
    for (int i = 0; i < defaultRecords.count(); i++)
    {
        DBRecord ri = defaultRecords.at(i);
        if (ri.at(Columns::Code).toInt() == code)
        {
            qDebug() << "@@@@@ SettingDBTable::checkDefault add default record " << code;
            resultRecords.append(ri);
            return;
        }
    }
}


