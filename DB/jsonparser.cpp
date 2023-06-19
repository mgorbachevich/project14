#include <QJsonObject>
#include <QJsonArray>
#include "database.h"
#include "jsonparser.h"
#include "tools.h"

DBRecordList JSONParser::parseTable(DBTable* table, const QJsonArray& recordValues)
{
    qDebug() << "@@@@@ JSONParser::parseTable ";

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

int JSONParser::parseAllTables(DataBase* db, const QString& json)
{
    qDebug() << "@@@@@ JSONParser::parseAllTables " << json;
    bool ok;
    QJsonValue data = prepare(json, &ok);
    int result = 0;
    if(ok)
    {
        for (int i = 0; i < db->tables.size(); i++)
        {
            DBTable* ti = db->tables[i];
            QJsonValue jv = data.toObject()[ti->name];
            if (jv.isArray())
                result += db->insertRecords(ti, parseTable(ti, jv.toArray()));
        }
    }
    qDebug() << "@@@@@ JSONParser::parseAllTables: result =" << result;
    return result;
}

DBRecordList JSONParser::parseTable(DBTable *table, const QString &json)
{
    qDebug() << "@@@@@ JSONParser::parseTable " << json;
    bool ok;
    QJsonValue jv = prepare(json, &ok).toObject()[table->name];
    if (ok && jv.isArray())
        return parseTable(table, jv.toArray());
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
