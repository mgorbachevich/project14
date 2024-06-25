#include "netactionresult.h"
#include "tools.h"
#include "appmanager.h"

NetActionResult::~NetActionResult()
{
    appManager->onNetResult(*this);
}

QString NetActionResult::makeJson()
{
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\"}"). arg(QString::number(errorCode), description);
    Tools::debugLog("NetActionResult::makeJson " + s);
    return s;
}

QString NetActionResult::makeJson(const QString& tableName, const QString& data)
{
    if(tableName.isEmpty() || data.isEmpty()) return makeJson();
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\",\"data\":{\"%3\":%4}}").
                    arg(QString::number(errorCode), description, tableName, data);
    Tools::debugLog("NetActionResult::makeJson " + s);
    return s;
}

QString NetActionResult::makeJson(const QString& data)
{
    if(data.isEmpty()) return makeJson();
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\",%3}").
                    arg(QString::number(errorCode), description, data);
    Tools::debugLog("NetActionResult::makeJson " + s);
    return s;
}

QString NetActionResult::makeJson(const QString& tableName, const QStringList& values)
{
    QString data;
    if(values.count() > 0)
    {
        data += QString("%1").arg(values[0]);
        for(int i = 1; i < values.count(); i++) data += QString(",%1").arg(values[i]);
    }
    QString s = QString("{\"result\":\"%1\",\"description\":\"%2\",\"codelist\":{\"%3\":[%4]}}").
        arg(QString::number(errorCode), description, tableName, data);
    Tools::debugLog("NetActionResult::makeJson " + s);
    return s;
}


