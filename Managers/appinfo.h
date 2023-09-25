#ifndef APPINFO_H
#define APPINFO_H

#include <QString>

#define APP_VERSION "1.44"

class AppInfo
{
public:
    AppInfo() {}
    QString all()
    {
        return QString("Приложение %1. БД %2. ВМ %3. Принтер %4. Сервер %5. IP %6").
                arg(appVersion, dbVersion, weightManagerVersion, printManagerVersion, netServerVersion, ip);
    }

    QString appVersion = APP_VERSION;
    QString dbVersion = "";
    QString weightManagerVersion = "";
    QString printManagerVersion = "";
    QString netServerVersion = "";
    QString ip = "";
};

#endif // APPINFO_H
