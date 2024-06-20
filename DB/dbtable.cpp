#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include "dbtable.h"
#include "tools.h"

QVariantList DBTable::createRecord(const QString code)
{
    DBRecord r;
    for (int i = 0; i < columnCount(); i++) r << QVariant("");
    if(!code.isEmpty()) r[0] = code;
    return r;
}

void DBTable::addColumn(const QString& title, const QString& name, const QString& type)
{
    columns.append(DBTableColumn(title, name, type));
}

const DBRecord DBTable::checkRecord(const DBRecord& record)
{
    DBRecord result;
    if(record.count() >= columnCount()) result.append(record);
    else Tools::debugLog("@@@@@ DBTable::checkRecord ERROR");
    return result;
}

bool DBTable::isEqual(const DBRecord& r1, const DBRecord& r2)
{
    if (r1.count() == r2.count())
    {
        for (int i = 0; i < r1.count(); i++) if (r1[i] != r2[i]) return false;
        return true;
    }
    return false;
}

int DBTable::columnIndex(const QString& columnName)
{
    for(int i = 0; i < columns.size(); i++) if(columns[i].name == columnName) return i;
    return -1;
}

const DBRecordList DBTable::checkList(DataBase*, const DBRecordList &records)
{
    Tools::debugLog("@@@@@ DBTable::checkList " + name);
    DBRecordList result;
    for (int i = 0; i < records.count(); i++)
    {
        const DBRecord& ri = checkRecord(records[i]);
        if (!ri.isEmpty()) result.append(ri);
    }
    return result;
}

QString DBTable::toJsonString(DBTable *table, const DBRecord &record)
{
    QString json;
    if (table != nullptr)
    {
        for (int i = 0; i < record.count() && i < table->columnCount(); i++)
        {
            if (i > 0) json += ",";
            json += QString("\"%1\":\"%2\"").arg(table->columnName(i), record.at(i).toString());
        }
    }
    return "{" + json + "}";
}

QString DBTable::toJsonString(DBTable *table, const DBRecordList &records)
{
    QString json;
    if (table != nullptr)
    {
        for (int i = 0; i < records.count(); i++)
        {
            if (i > 0) json += ",";
            json += toJsonString(table, records.at(i));
        }
    }
    return "[" + json + "]";
}

QString DBTable::columnTitle(const int index) const
{
    return (index < columns.count() && index >= 0) ? columns[index].title : "";
}

QString DBTable::columnName(const int index) const
{
    return (index < columns.count() && index >= 0) ? columns[index].name : "";
}

QString DBTable::columnType(const int index) const
{
    return (index < columns.count() && index >= 0) ? columns[index].type : "";
}

void DBTable::parseColumn(DBRecord& r, const QJsonObject &jo, const int columnIndex)
{
    QString type = columnType(columnIndex);
    QString value = jo[columnName(columnIndex)].toString("");
    if (type.contains("INT"))       r[columnIndex] = Tools::stringToInt(value);
    else if (type.contains("REAL")) r[columnIndex] = Tools::stringToDouble(value);
    else                            r[columnIndex] = value;
}

DBRecordList DBTable::parse(const QString &json)
{
    DBRecordList records;
    const QJsonObject jo = Tools::stringToJson(json);
    QJsonValue data = jo["data"];
    if (!data.isObject())
    {
        Tools::debugLog("@@@@@ DBTable::parse ERROR !data.isObject()");
        return records;
    }
    QJsonValue jt = data.toObject()[name];
    if(!jt.isArray()) return records;
    Tools::debugLog("@@@@@ DBTable::parse " + name);
    QJsonArray ja = jt.toArray();
    for (int i = 0; i < ja.size(); i++)
    {
        QJsonValue jai = ja[i];
        if (jai.isObject())
        {
            DBRecord r = createRecord();
            for (int j = 0; j < columnCount(); j++) parseColumn(r, jai.toObject(), j);
            records << r;
        }
    }
    Tools::debugLog(QString("@@@@@ DBTable::parse Done %1").arg(records.count()));
    return records;
}

