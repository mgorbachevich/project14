#include <QJsonDocument>
#include "jsonarrayfile.h"
#include "tools.h"

JsonArrayFile::JsonArrayFile(const QString &file, AppManager* parent) : JsonFile(file, parent)
{
    Tools::debugLog("@@@@@ JsonArrayFile::JsonArrayFile " + fileName);
}

bool JsonArrayFile::read()
{
    items = parse(Tools::readTextFile(fileName));
    parseDefault();
    sort();
    Tools::debugLog("@@@@@ JsonArrayFile::read " + Tools::intToString(items.count()));
    return items.count() > 0;
}

bool JsonArrayFile::insertOrReplace(const QString& json)
{
    getAll();
    DBRecordList records = parse(json);
    int n = records.count();
    int remove = 0;
    for(int i = 0; i < n; i++)
    {
        const int code = records[i][0].toInt();
        for(int j = 0; j < items.count(); j++)
            if(code == items[j][0].toInt()) { items.removeAt(j); remove++; break; }
        items << records[i];
    }
    if(n > 0)
    {
        parseDefault();
        sort();
    }
    Tools::debugLog(QString("@@@@@ JsonArrayFile::insertOrReplace %1 %2 %3").arg(
        Tools::intToString(n), Tools::intToString(remove), Tools::intToString(items.count())));
    return n > 0;
}

bool JsonArrayFile::write()
{
    sort();
    return JsonFile::write();
}

DBRecordList JsonArrayFile::parse(const QString& json)
{
    DBRecordList records;
    const QJsonObject jo = QJsonDocument::fromJson(json.toUtf8()).object();
    QJsonValue data = jo[mainObjectName];
    if (!data.isObject())
    {
        Tools::debugLog("@@@@@ JsonArrayFile::parse error !data.isObject()");
        return records;
    }
    QJsonValue jv = data.toObject()[itemArrayName];
    if(!jv.isArray())
    {
        Tools::debugLog("@@@@@ JsonArrayFile::parse error !jv.isArray()");
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

DBRecord *JsonArrayFile::getByCode(const int code)
{
    getAll();
    bool ok = false;
    for (DBRecord& r : items) if (r[0].toInt(&ok) == code && ok) return &r;
    return nullptr;
}

QJsonObject JsonArrayFile::toJson()
{
    getAll();
    QJsonArray ja;
    for (DBRecord& r : items)
    {
        QJsonObject ji;
        for(int i = 0; i < r.count() && i < fields.count(); i++)
            ji.insert(fields.value(i), r.at(i).toJsonValue());
        ja.append(ji);
    }
    QJsonObject jo;
    jo[itemArrayName] = ja;
    QJsonObject result;
    result[mainObjectName] = jo;
    return result;
}


