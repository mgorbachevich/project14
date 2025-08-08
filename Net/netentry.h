#ifndef NETENTRY_H
#define NETENTRY_H

#include <QString>
#include <QNetworkInterface>

class NetEntry
{
public:
    explicit NetEntry() {}

    bool isIP() const { return !ip.isEmpty(); }
    bool isWiFi() const;
    bool isEthernet() const;

    QString ip;
    QString type;
    QString ssid;
};

#endif // NETENTRY_H
