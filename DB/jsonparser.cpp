#include <QJsonObject>
#include <QJsonArray>
#include "database.h"
#include "jsonparser.h"
#include "tools.h"

DBRecordList JSONParser::parseArray(DBTable* table, const QJsonArray& recordValues)
{
    qDebug() << "@@@@@ JSONParser::parseArray ";

    DBRecordList records;
    for (int i = 0; i < recordValues.size(); i++)
    {
        QJsonValue jv = recordValues[i];
        if (jv.isObject())
        {
            DBRecord r = table->createRecord();
            for (int j = 0; j < table->columnCount(); j++)
                parseTableColumn(table, r, jv.toObject(), j);
            records << r;
        }
    }
    return records;
}

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

DBRecordList JSONParser::parseTable(DBTable *table, const QString &json)
{
    qDebug() << "@@@@@ JSONParser::parseTable " << json;
    bool ok;
    QJsonValue jv = prepare(json, &ok).toObject()[table->name];
    if (ok && jv.isArray())
        return parseArray(table, jv.toArray());
    return DBRecordList();
}

QJsonValue JSONParser::prepare(const QString &json, bool *ok)
{
    qDebug() << "@@@@@ JSONParser::prepare " << json;

    const QJsonObject jo = Tools::stringToJson(json);
    QJsonValue result = jo["result"];
    /*
    if (!result.isString() || Tools::stringToInt(result.toString()) != 0)
    {
        qDebug() << "@@@@@ JSONParser::prepare result " << result.toString("") << jo["description"].toString("");
        *ok = false;
        return QJsonValue();
    }
    */
    QJsonValue data = jo["data"];
    if (!data.isObject())
    {
        qDebug() << "@@@@@ JSONParser::prepare ERROR (data is not object)";
        *ok = false;
        return QJsonValue();
    }
    *ok = true;
    return data;
}

int JSONParser::parseAllTables(DataBase* db, const QString& json, int* returnErrorCode, QString* returnDescription)
{
    qDebug() << "@@@@@ JSONParser::parseAllTables " << json;
    db->saveLog(LogType_Error, LogSource_DB, "Загрузка");

    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    int logging = db->settings.getItemIntValue(SettingDBTable::SettingCode_Logging);
    bool detailedLog = logging >= LogType_Info && (returnErrorCode != nullptr || returnDescription != nullptr);
    bool ok;

    QJsonValue jsonData = prepare(json, &ok);
    if(!ok)
    {
        errorCount = 1;
        errorCode = LogError_WrongRequest;
        description = "Некорректный запрос";
    }
    else
    {
        for (int i = 0; i < db->tables.size(); i++)
        {
            DBTable* ti = db->tables[i];
            QJsonValue jv = jsonData.toObject()[ti->name];
            if (jv.isArray())
            {
                DBRecordList records = parseArray(ti, jv.toArray());
                for (int j = 0; j < records.count(); j++)
                {
                    DBRecord& rj = records[j];
                    if(db->insertRecord(ti, rj))
                    {
                        recordCount++;
                        if (detailedLog)
                        {
                            QString s = QString("Запись загружена. Таблица: %1. Код: %2").arg(ti->name, rj.at(0).toString());
                            db->saveLog(LogType_Warning, LogSource_DB, s);
                        }
                    }
                    else
                    {
                        errorCount++;
                        errorCode = LogError_WrongRecord;
                        description = "Некорректная запись";
                        if (detailedLog)
                        {
                            QString s = QString("Ошибка загрузки записи. Таблица: %1. Код: %2. Код ошибки: %3. Описание: %4").
                                    arg(ti->name, rj.at(0).toString(), QString::number(errorCode), description);
                            db->saveLog(LogType_Error, LogSource_DB, s);
                        }
                    }
                }
            }
        }
    }
    QString s = QString("Загрузка завершена. Записи: %2. Ошибки: %3. Описание: %4").
            arg(QString::number(recordCount), QString::number(errorCount), description);
    db->saveLog(LogType_Error, LogSource_DB, s);
    qDebug() << "@@@@@ JSONParser::parseAllTables: result =" << recordCount;

    if(returnErrorCode != nullptr) *returnErrorCode = errorCode;
    if(returnDescription != nullptr) *returnDescription = description;

    return recordCount;
}

