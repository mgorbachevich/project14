#include <QJsonArray>
#include <QJsonDocument>
#include "jsonarrayfile.h"
#include "tools.h"

JsonArrayFile::JsonArrayFile(const QString &file, QObject* parent) : JsonFile(file, parent)
{
    Tools::debugLog("@@@@@ JsonArrayFile::JsonArrayFile " + fileName);
}

bool JsonArrayFile::read()
{
    if(!wasRead)
    {
        wasRead = true;
        items = parse(Tools::readTextFile(fileName));
        sort();
        Tools::debugLog("@@@@@ JsonArrayFile::read " + Tools::toString((int)(items.count())));
    }
    return items.count() > 0;
}

bool JsonArrayFile::insertOrReplace(const QString& json)
{
    getAll();
    DBRecordList records = parse(json);
    const int n = records.count();
    for(int i = 0; i < n; i++)
    {
        DBRecord& ri = records[i];
        const int code = ri[0].toInt();
        bool found = false;
        for(int j = 0; j < items.count() && !found; j++)
        {
            if(code ==  items[j][0].toInt())
            {
                found = true;
                for(int k = 0; k < ri.count() && k < items[j].count(); k++)
                {
                    if(!ri[k].isNull() && ri[k].isValid() &&
                       !ri[k].toString().isNull() && !ri[k].toString().isEmpty())
                        items[j][k] = ri[k];
                }
            }
        }
        if(!found) items << records[i];
    }
    if(n > 0) sort();
    Tools::debugLog(QString("@@@@@ JsonArrayFile::insertOrReplace %1 %2").arg(
        Tools::toString(n), Tools::toString((int)(items.count()))));
    return n > 0;
}

bool JsonArrayFile::insertOrReplaceRecord(const DBRecord& r)
{
    if(r.isEmpty()) return false;
    const int code = r[0].toInt();
    Tools::debugLog("@@@@@ JsonArrayFile::insertOrReplace " + Tools::toString(code));
    DBRecord* p = getByCode(code);
    if(p != nullptr) *p = r;
    else items << r;
    return true;
}

void JsonArrayFile::removeByCode(const QString& code)
{
    Tools::debugLog("@@@@@ JsonArrayFile::removeByCode " + code);
    const int ic = Tools::toInt(code);
    if(getByCode(ic) != nullptr)
    {
        const int i = getIndex(ic);
        if(!getByIndex(i).isEmpty()) items.remove(i);
    }
}

bool JsonArrayFile::write()
{
    sort();
    return JsonFile::write();
}

DBRecordList JsonArrayFile::parse(const QString& json)
{
    DBRecordList records;
    const QJsonObject jo = Tools::toJsonObject(json);
    QJsonValue data = jo["data"];
    if (!data.isObject())
    {
        Tools::debugLog("@@@@@ JsonArrayFile::parse ERROR !data.isObject()");
        return records;
    }
    QJsonValue jv = data.toObject()[itemArrayName];
    if(!jv.isArray())
    {
        Tools::debugLog("@@@@@ JsonArrayFile::parse error");
        return records;
    }
    QJsonArray ja = jv.toArray();
    for (int i = 0; i < ja.size(); i++)
    {
        QJsonValue ji = ja[i];
        if (ji.isObject())
        {
            DBRecord r;
            for (int j = 0; j < fields.count(); j++) r << ji[fields.value(j)].toString("");
            records << r;
        }
    }
    return records;
}

int JsonArrayFile::getIndex(const int code)
{
    for (int i = 0; i < items.count(); i++) if(items[i][0].toInt() == code) return i;
    return -1;
}

DBRecordList JsonArrayFile::getAll()
{
    if(items.count() == 0) read();
    return items;
}

DBRecord *JsonArrayFile::getByCode(const quint64 code)
{
    getAll();
    bool ok = false;
    for (DBRecord& r : items) if (! r.isEmpty() && r[0].toULongLong(&ok) == code && ok) return &r;
    return nullptr;
}

QJsonObject JsonArrayFile::toJsonObject()
{
    getAll();
    QJsonArray ja;
    for (DBRecord& r : items) appendItemToJson(r, ja);
    QJsonObject jo;
    jo.insert(itemArrayName, ja);
    return jo;
}

void JsonArrayFile::appendItemToJson(DBRecord& r, QJsonArray& ja)
{
    QJsonObject ji;
    for(int i = 0; i < r.count() && i < fields.count(); i++)
        ji.insert(fields.value(i), r.at(i).toJsonValue());
    ja.append(ji);
}

