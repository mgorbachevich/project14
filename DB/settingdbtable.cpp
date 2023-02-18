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
    QList<int> foundCodes;

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
        case SettingCode::SettingCode_ScalesNumber:
            if (value < 1 || value > 999999) continue;
            break;
        case SettingCode::SettingCode_ScalesName:
        case SettingCode::SettingCode_ShopName:
        case SettingCode::SettingCode_TCPPort:
            break;
        default:
            continue;
        }
        foundCodes.append(code);
        resultRecords.append(ri);
    }

    // Добавление недостающих значений по умолчанию из файла json_default_settings.txt:
    JSONParser parser;
    DBRecordList defaultRecords = parser.run(this, Tools::readTextFile(":/Text/json_default_settings.txt"));
    checkDefault(SettingCode::SettingCode_ScalesNumber, foundCodes, defaultRecords, resultRecords);
    checkDefault(SettingCode::SettingCode_TCPPort, foundCodes, defaultRecords, resultRecords);

    return resultRecords;
}

void SettingDBTable::checkDefault(const SettingCode code, const QList<int>& foundCodes, const DBRecordList& defaultRecords, DBRecordList& resultRecords)
{
    if (!foundCodes.contains(code))
    {
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
}
