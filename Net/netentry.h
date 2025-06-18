#ifndef NETENTRY_H
#define NETENTRY_H

#include <QString>
#include <QNetworkInterface>

class NetEntry
{
public:
    NetEntry() {}

    bool isWiFi()      { return 0 == type.compare("WIFI", Qt::CaseInsensitive); }
    bool isEthernet()  { return 0 == type.compare("ETHERNET", Qt::CaseInsensitive); }

    QString ip;
    QString type;
    QString ssid;
};

#endif // NETENTRY_H
