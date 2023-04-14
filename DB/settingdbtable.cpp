#include "settingdbtable.h"
#include "tools.h"
#include "jsonparser.h"

SettingDBTable::SettingDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    qDebug() << "@@@@@ SettingDBTable::SettingDBTable";

    addColumn("Код",          "code",  "INT PRIMARY KEY");
    addColumn("Наименование", "name",  "TEXT");
    addColumn("Значение",     "value", "TEXT");
}

const DBRecordList SettingDBTable::checkAll(const DBRecordList& records)
{
    qDebug() << "@@@@@ SettingDBTable::checkAll";
    DBRecordList resultRecords;

    // Проверка допустимых значений:
    for (int i = 0; i < records.count(); i++)
    {
        const DBRecord& ri = records.at(i);
        if (ri.count() != columnCount()) continue;
        bool ok = false;
        int code = ri.at(Columns::Code).toUInt(&ok);
        if (!ok) continue;
        int value = ri.at(Columns::Value).toInt(&ok);
        if (!ok) value = 0;
        switch (code)
        {
        case SettingCode_ScalesNumber:
            if (value < 1 || value > 999999) continue;
            break;
        case SettingCode_PointPosition:
            if (value < 0 || value > 3) continue;
            break;
        case SettingCode_ProductReset:
            if (value < ProductReset_None || value > ProductReset_Time) continue;
            break;
        case SettingCode_ProductResetTime:
            if (value < 0) continue;
            break;
        case SettingCode_ScalesName:
        case SettingCode_ShopName:
        case SettingCode_TCPPort:
        case SettingCode_LogDuration:
            break; // Без проверки
        default: continue;
        }
        resultRecords.append(ri);
    }

    // Добавление недостающих значений по умолчанию:
    JSONParser parser;
    DBRecordList defaultRecords = parser.parseTable(this, Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    checkDefault(SettingCode_ScalesNumber, defaultRecords, resultRecords);
    checkDefault(SettingCode_TCPPort, defaultRecords, resultRecords);
    checkDefault(SettingCode_PointPosition, defaultRecords, resultRecords);
    checkDefault(SettingCode_ProductReset, defaultRecords, resultRecords);
    checkDefault(SettingCode_ProductResetTime, defaultRecords, resultRecords);
    checkDefault(SettingCode_LogDuration, defaultRecords, resultRecords);

    return resultRecords;
}

void SettingDBTable::checkDefault(const SettingCode code, const DBRecordList& defaultRecords, DBRecordList& resultRecords)
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
