#ifndef NETACTIONRESULT_H
#define NETACTIONRESULT_H

#include <QString>
#include <QDateTime>
#include "constants.h"

class AppManager;

class NetActionResult
{
public:
    NetActionResult(const RouterRule);

    QString makeEmptyJson();
    QString makeJson(const QJsonObject&);
    QString makeCodeListJson(const QString&, const QStringList&);

    int successCount;
    int recordCount;
    int errorCount;
    int errorCode;
    QString description ;
    QString requestReply;
    RouterRule rule;

protected:
    void onMakeJson();
};

#endif // NETACTIONRESULT_H
