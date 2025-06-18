#include <QJsonDocument>
#include <QJsonArray>
#include "netactionresult.h"
#include "tools.h"
#include "appmanager.h"

NetActionResult::NetActionResult(const RouterRule r)
{
    rule = r;
    successCount = 0;
    recordCount = 0;
    errorCount = 0;
    errorCode = 0;
    description = "Ошибок нет";
    requestReply = "";
}

QString NetActionResult::makeEmptyJson()
{
    QJsonObject jo;
    jo.insert("result",      QJsonValue(QString::number(errorCode)));
    jo.insert("description", QJsonValue(description));
    QString s = Tools::toString(jo);
    Tools::debugLog("NetActionResult::makeEmptyJson " + s);
    return s;
}

QString NetActionResult::makeJson(const QJsonObject& data)
{
    if(data.isEmpty()) return makeEmptyJson();
    /*
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\",%3}").
                    arg(QString::number(errorCode), description, data);
    */
    QJsonObject jo;
    jo.insert("result",      QJsonValue(QString::number(errorCode)));
    jo.insert("description", QJsonValue(description));
    jo.insert("data",        data);
    QString s = Tools::toString(jo);
    Tools::debugLog("NetActionResult::makeJson " + s);
    return s;
}

QString NetActionResult::makeCodeListJson(const QString& tableName, const QStringList& codes)
{
    /*
    QString data;
    if(values.count() > 0)
    {
        data += QString("%1").arg(values[0]);
        for(int i = 1; i < values.count(); i++) data += QString(",%1").arg(values[i]);
    }
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\",\"codelist\":{\"%3\":[%4]}}").
        arg(QString::number(errorCode), description, tableName, data);
    */

    QVariantList vl;
    for(int i = 0; i < codes.count(); i++) vl += Tools::toInt(codes[i]);
    QJsonObject jo1;
    jo1.insert(tableName, QJsonArray::fromVariantList(vl));

    QJsonObject jo;
    jo.insert("result",      QJsonValue(QString::number(errorCode)));
    jo.insert("description", QJsonValue(description));
    jo.insert("codelist",    jo1);
    QString s = Tools::toString(jo);
    Tools::debugLog("NetActionResult::makeCodeListJson " + s);
    return s;
}


