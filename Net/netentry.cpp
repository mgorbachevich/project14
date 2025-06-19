#include "netentry.h"

bool NetEntry::isWiFi() const
{
    if(!isIP()) return false;
    if(type == "Ethernet") return false;
    if(type == "Wifi") return true;
    return !ssid.isEmpty();
}

bool NetEntry::isEthernet() const
{
    if(!isIP()) return false;
    if(type == "Ethernet") return true;
    if(type == "Wifi") return false;
    return ssid.isEmpty();
}
