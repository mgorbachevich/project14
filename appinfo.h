#ifndef APPINFO_H
#define APPINFO_H

#include <QString>
#include "constants.h"

class AppInfo
{
public:
    AppInfo() {}
    QString all(const QString delimeter)
    {
        return QString("Штрих ПРИНТ 6А %1.%2БД %3.%4ВМ %5.%6Принтер %7.%8Сервер %9.%10IP %11").
                arg(appVersion, delimeter,
                    dbVersion, delimeter,
                    weightManagerVersion, delimeter,
                    printManagerVersion, delimeter,
                    netServerVersion, delimeter,
                    ip);
    }

    QString appVersion = APP_VERSION;
    QString dbVersion = "";
    QString weightManagerVersion = "";
    QString printManagerVersion = "";
    QString netServerVersion = "";
    QString ip = "";
};

#endif // APPINFO_H
