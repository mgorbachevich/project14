#include <QJsonObject>
#include <QJsonArray>
#include "database.h"
#include "jsonparser.h"
#include "tools.h"

void JSONParser::parseTableColumn(DBTable* table, DBRecord& r, const QJsonObject &jo, const int columnIndex)
{
    // qDebug() << "@@@@@ JSONParser::parseTableColumn " << columnIndex;
    QString type = table->columnType(columnIndex);
    QString value = jo[table->columnName(columnIndex)].toString("");
    if (type.contains("INT"))
        r[columnIndex] = Tools::stringToInt(value);
    else if (type.contains("REAL"))
        r[columnIndex] = Tools::stringToDouble(value);
    else // if (type.contains("TEXT"))
        r[columnIndex] = value;
}

DBRecordList JSONParser::parseTable(DBTable *table, const QString &json, bool *ok)
{
    DBRecordList records;
    if(table == nullptr)
    {
        qDebug() << "@@@@@ JSONParser::parseTable ERROR (table == nullptr)";
        if(ok != nullptr) *ok = false;
        return records;
    }
    qDebug() << "@@@@@ JSONParser::parseTable " << table->name;
    const QJsonObject jo = Tools::stringToJson(json);
    QJsonValue data = jo["data"];
    if (!data.isObject())
    {
        qDebug() << "@@@@@ JSONParser::parseTable ERROR (!data.isObject())";
        if(ok != nullptr) *ok = false;
        return records;
    }
    if(ok != nullptr) *ok = true;
    QJsonValue jt = data.toObject()[table->name];
    if(!jt.isArray())
    {
        qDebug() << "@@@@@ JSONParser::parseTable ERROR (!jt.isArray())";
        return records;
    }
    QJsonArray ja = jt.toArray();
    for (int i = 0; i < ja.size(); i++)
    {
        QJsonValue jai = ja[i];
        if (jai.isObject())
        {
            DBRecord r = table->createRecord();
            for (int j = 0; j < table->columnCount(); j++) parseTableColumn(table, r, jai.toObject(), j);
            records << r;
        }
    }
    qDebug() << "@@@@@ JSONParser::parseTable Done " << records.count();
    return records;
}
/*
void JSONParser::parseAndSaveAllTables(DataBase* db, const QString& json, int* returnErrorCode, QString* returnDescription)
{
    qDebug() << "@@@@@ JSONParser::parseAndSaveAllTables ";
    //qDebug() << "@@@@@ JSONParser::parseAndSaveAllTables " << json;
    db->saveLog(LogType_Error, LogSource_DB, "Загрузка");

    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    int logging = db->settings.getItemIntValue(SettingDBTable::SettingCode_Logging);
    bool detailedLog = (logging >= LogType_Info) && (returnErrorCode != nullptr || returnDescription != nullptr);
    for (int i = 0; i < db->tables.size(); i++)
    {
        DBTable* ti = db->tables[i];
        bool ok;
        DBRecordList records = parseTable(ti, json, &ok);
        if(ok) for (int j = 0; j < records.count(); j++)
        {
            DBRecord& rj = records[j];
            QString code = rj.count() > 0 ? rj.at(0).toString() : "";
            QString s;
            if(code.isEmpty() || !db->insertRecord(ti, rj))
            {
                errorCount++;
                errorCode = LogError_WrongRecord;
                description = "Некорректная запись";
                s = QString("Ошибка загрузки записи. Таблица: %1. Код: %2. Код ошибки: %3. Описание: %4").
                        arg(ti->name, code, QString::number(errorCode), description);
                if (detailedLog) db->saveLog(LogType_Error, LogSource_DB, s);
                qDebug() << "@@@@@ JSONParser::parseAndSaveAllTables " << s;
            }
            else
            {
                recordCount++;
                s = QString("Запись загружена. Таблица: %1. Код: %2").arg(ti->name, code);
                if (detailedLog) db->saveLog(LogType_Warning, LogSource_DB, s);
            }
            qDebug() << "@@@@@ JSONParser::parseAndSaveAllTables " << s;
        }
    }
    QString s = QString("Загрузка завершена. Записи: %2. Ошибки: %3. Описание: %4").
            arg(QString::number(recordCount), QString::number(errorCount), description);
    db->saveLog(LogType_Error, LogSource_DB, s);
    qDebug() << "@@@@@ JSONParser::parseAndSaveAllTables " << s;
    if(returnErrorCode != nullptr) *returnErrorCode = errorCode;
    if(returnDescription != nullptr) *returnDescription = description;
    return recordCount;
}
*/

