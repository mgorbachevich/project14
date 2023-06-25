#ifndef APPINFO_H
#define APPINFO_H

#include <QString>

class AppInfo
{
public:
    AppInfo() {}
    QString all()
    {
        return QString("Приложение %1. БД %2. ВМ %3. Принтер %4. Сервер %5. IP %6").
                arg(appVersion, dbVersion, weightManagerVersion, printManagerVersion, netServerVersion, ip);
    }

    QString appVersion = "";
    QString dbVersion = "";
    QString weightManagerVersion = "";
    QString printManagerVersion = "";
    QString netServerVersion = "";
    QString ip = "";
};

#endif // APPINFO_H
