#ifndef NETACTIONRESULT_H
#define NETACTIONRESULT_H

#include <QString>
#include <QDateTime>
#include "constants.h"

class AppManager;

class NetActionResult
{
public:
    NetActionResult(AppManager* am, const RouterRule r = RouterRule_Set) { appManager = am; rule = r; }
    ~NetActionResult();

    QString makeJson();
    QString makeJson(const QString&);
    QString makeJson(const QString&, const QString&);
    QString makeJson(const QString&, const QStringList&);

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
