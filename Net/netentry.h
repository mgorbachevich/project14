#ifndef NETENTRY_H
#define NETENTRY_H

#include <QString>
#include <QNetworkInterface>
#include "constants.h"

class NetEntry
{
public:
    NetEntry() {}

    bool isWiFi()      { return type == QNetworkInterface::Wifi; }
    bool isEthernet()  { return type == QNetworkInterface::Ethernet; }
    QString typeName() { if(isWiFi()) return "Wifi"; if(isEthernet()) return "Ethernet"; return ""; }

    QString ip;
    int type = UNKNOWN;
    QString ssid;
};

#endif // NETENTRY_H
