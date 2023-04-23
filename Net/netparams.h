#ifndef NETPARAMS_H
#define NETPARAMS_H

#include <QString>

class NetParams
{
public:
    NetParams() {}

    QString localHostName;
    QString localHostIP;
    QString localMacAddress;
    QString localNetMask;
};

#endif // NETPARAMS_H
