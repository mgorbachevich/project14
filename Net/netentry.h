#ifndef NETENTRY_H
#define NETENTRY_H

#include <QString>

class NetEntry
{
public:
    NetEntry() {}

    bool isWiFi()     { return 0 == type.compare("WiFi", Qt::CaseInsensitive); }
    bool isEthernet() { return 0 == type.compare("Ethernet", Qt::CaseInsensitive); }

    QString ip;
    QString type;
    QString ssid;
};

#endif // NETENTRY_H
