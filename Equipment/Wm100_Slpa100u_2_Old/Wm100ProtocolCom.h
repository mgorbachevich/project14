#ifndef WM100PROTOCOLCOM_H
#define WM100PROTOCOLCOM_H

#include <QObject>
#include "Wm100Protocol.h"

class Wm100ProtocolCom : public Wm100Protocol
{
    Q_OBJECT
public:
    explicit Wm100ProtocolCom(QObject *parent = nullptr);
    enum t_protocolSymbol{
        ENQ  = 0x05,
        STX  = 0x02,
        ACK  = 0x06,
        NAK  = 0x15,
        SYNC = 0x16
    };
    virtual int open(const QString &uri);

public:
    static bool checkUri(const QString &uri);

private:
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in);
    char crc(const QByteArray &buf);
};

#endif // WM100PROTOCOLCOM_H
