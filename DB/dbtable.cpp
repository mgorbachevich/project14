#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include "dbtable.h"
#include "tools.h"
#include "database.h"

DBTable::DBTable(const QString &name, QSqlDatabase &sqlDB, DataBase *parent) :
    QObject((QObject*)parent), name(name), sqlDB(sqlDB), db(parent) {}

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

const DBRecordList DBTable::checkList(const DBRecordList &records)
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

QJsonObject DBTable::toJsonObject(DBTable *table, const DBRecord &record)
{
    QJsonObject jo;
    if (table != nullptr)
    {
        for (int i = 0; i < record.count() && i < table->columnCount(); i++)
        {
            if(table->notUploadColumns.contains(i)) continue;
            jo.insert(table->columnName(i), QJsonValue(record.at(i).toString()));
        }
    }
    return jo;
}

QString DBTable::toJsonString(DBTable *table, const DBRecord &record)
{
    QString json;
    if (table != nullptr)
    {
        for (int i = 0; i < record.count() && i < table->columnCount(); i++)
        {
            if(table->notUploadColumns.contains(i)) continue;
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

QJsonObject DBTable::toJsonObject(DBTable *table, const DBRecordList &records)
{
    QJsonArray ja;
    QJsonObject jo;
    if (table != nullptr)
    {
        for (int i = 0; i < records.count(); i++)
        {
            ja.append(toJsonObject(table, records.at(i)));
        }
        jo.insert(table->name, ja);
    }
    return jo;
}

QString DBTable::columnTitle(const int i) const
{
    return (i < columns.count() && i >= 0) ? columns[i].title : "";
}

QString DBTable::columnName(const int i) const
{
    return (i < columns.count() && i >= 0) ? columns[i].name : "";
}

QString DBTable::columnType(const int i) const
{
    return (i < columns.count() && i >= 0) ? columns[i].type : "";
}

void DBTable::parseColumn(DBRecord& r, const QJsonObject &jo, const int i)
{
    QString type = columnType(i);
    QString value = jo[columnName(i)].toString("");
    if (type.contains("INT"))       r[i] = Tools::toInt(value);
    else if (type.contains("REAL")) r[i] = Tools::toDouble(value);
    else                            r[i] = value;
}

DBRecordList DBTable::parse(const QString &json)
{
    DBRecordList records;
    const QJsonObject jo = Tools::toJsonObject(json);
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

void DBTable::removeIndexes()
{
    if (!db->isStarted() || indexDescriptors.count() < 1) return;
    Tools::debugLog("DBTable::removeIndexes");
    for(int i = 0; i < indexDescriptors.count(); i++)
        db->executeSQL(sqlDB, QString("DROP INDEX IF EXISTS %1").arg(indexDescriptors[i].name));
}

QString DBTable::toString(DBRecord& r)
{
    QString s;
    for(int i = 0; i < columns.count(); i++)
    {
        if(i > 0) s += ", ";
        if(i >= r.count()) s += "";
        else s += r[i].toString();
    }
    return s;
}

void DBTable::fill(DBRecord& r)
{
    for(int i = r.count(); i < columns.count(); i++) r.append("");
}

void DBTable::createIndexes()
{
    if (!db->isStarted() || indexDescriptors.count() < 1) return;
    Tools::debugLog("DBTable::createIndexes");
    removeIndexes();
    for(int i = 0; i < indexDescriptors.count(); i++)
    {
        QString sql = QString("CREATE INDEX %1 ON %2 (%3) %4").arg(
                    indexDescriptors[i].name,
                    name,
                    columnName(indexDescriptors[i].column),
                    indexDescriptors[i].condition);
        db->executeSQL(sqlDB, sql);
    }
}

