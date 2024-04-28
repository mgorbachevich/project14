#include <QJsonDocument>
#include "jsonfile.h"
#include "tools.h"

JsonFile::JsonFile(const QString &file, QObject *parent): QObject(parent), fileName(file)
{
    Tools::debugLog("@@@@@ JsonFile::JsonFile " + fileName);
}

bool JsonFile::read()
{
    items = parse(Tools::readTextFile(fileName));
    parseDefault();
    sort();
    Tools::debugLog("@@@@@ JsonFile::read " + Tools::intToString(items.count()));
    return items.count() > 0;
}

bool JsonFile::insertOrReplace(const QString& json)
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
    Tools::debugLog(QString("@@@@@ JsonFile::insertOrReplace %1 %2 %3").arg(
        Tools::intToString(n), Tools::intToString(remove), Tools::intToString(items.count())));
    return n > 0;
}

bool JsonFile::write()
{
    bool ok = Tools::writeTextFile(fileName, toString());
    Tools::debugLog(QString("@@@@@ JsonFile::write %1 %2").arg(fileName, Tools::boolToString(ok)));
    if(!ok) message = "Ошибка записи файла " + fileName;
    else message = "Файл записан " + fileName;
    return ok;
}

QString JsonFile::getAndClearMessage()
{
    QString s = message;
    message = "";
    return s;
}

QString JsonFile::toString()
{
    QJsonDocument doc(toJson());
    return doc.toJson(QJsonDocument::Indented);  // QJsonDocument::Compact or QJsonDocument::Indented
}

DBRecordList JsonFile::parse(const QString& json)
{
    DBRecordList records;
    const QJsonObject jo = QJsonDocument::fromJson(json.toUtf8()).object();
    QJsonValue data = jo[mainObjectName];
    if (!data.isObject())
    {
        Tools::debugLog("@@@@@ JsonFile::parse error !data.isObject()");
        return records;
    }
    QJsonValue jv = data.toObject()[itemArrayName];
    if(!jv.isArray())
    {
        Tools::debugLog("@@@@@ JsonFile::parse error !jv.isArray()");
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

DBRecordList JsonFile::getAll()
{
    if(items.count() == 0) read();
    return items;
}

QJsonObject JsonFile::toJson()
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


