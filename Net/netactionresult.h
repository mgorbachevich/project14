#ifndef NETACTIONRESULT_H
#define NETACTIONRESULT_H

#include <QString>
#include <QtCore/qjsonobject.h>
#include "constants.h"

class AppManager;

class NetActionResult
{
public:
    NetActionResult(const RouterRule);

    QString makeEmptyJson();
    QString makeJson(const QJsonObject&);
    QString makeCodeListJson(const QString&, const QStringList&);

    int successCount = 0;
    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description;
    QString requestReply;
    RouterRule rule;

protected:
    void onMakeJson();
};

#endif // NETACTIONRESULT_H
