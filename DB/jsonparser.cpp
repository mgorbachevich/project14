#include <QJsonObject>
#include <QJsonArray>
#include "dbtable.h"
#include "jsonparser.h"
#include "tools.h"

void JSONParser::parseTableColumn(DBTable* table, DBRecord& r, const QJsonObject &jo, const int columnIndex)
{
    QString type = table->columnType(columnIndex);
    QString value = jo[table->columnName(columnIndex)].toString("");
    if (type.contains("INT"))       r[columnIndex] = Tools::stringToInt(value);
    else if (type.contains("REAL")) r[columnIndex] = Tools::stringToDouble(value);
    else                            r[columnIndex] = value;
}

DBRecordList JSONParser::parseTable(DBTable *table, const QString &json, bool *ok)
{
    DBRecordList records;
    if(table == nullptr)
    {
        Tools::debugLog("@@@@@ JSONParser::parseTable ERROR table==nullptr");
        if(ok != nullptr) *ok = false;
        return records;
    }
    const QJsonObject jo = Tools::stringToJson(json);
    QJsonValue data = jo["data"];
    if (!data.isObject())
    {
        Tools::debugLog("@@@@@ JSONParser::parseTable ERROR !data.isObject()");
        if(ok != nullptr) *ok = false;
        return records;
    }
    if(ok != nullptr) *ok = true;
    QJsonValue jt = data.toObject()[table->name];
    if(!jt.isArray()) return records;
    Tools::debugLog("@@@@@ JSONParser::parseTable " + table->name);
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
    Tools::debugLog(QString("@@@@@ JSONParser::parseTable Done %1").arg(records.count()));
    return records;
}
