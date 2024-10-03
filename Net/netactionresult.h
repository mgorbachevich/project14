#ifndef NETACTIONRESULT_H
#define NETACTIONRESULT_H

#include <QString>
#include <QDateTime>
#include "constants.h"

class AppManager;

class NetActionResult
{
public:
    NetActionResult(AppManager* am, const RouterRule r) { appManager = am; rule = r; }
    ~NetActionResult();

    QString makeEmptyJson();
    QString makeJson(const QJsonObject&);
    QString makeCodeListJson(const QString&, const QStringList&);

    int successCount = 0;
    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    RouterRule rule;

protected:
    void onMakeJson();

    AppManager* appManager;
};

#endif // NETACTIONRESULT_H
