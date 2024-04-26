#ifndef SLPA100UPROTOCOLCOM_H
#define SLPA100UPROTOCOLCOM_H

#include <QObject>
#include "Slpa100uProtocol.h"

class Slpa100uProtocolCom : public Slpa100uProtocol
{
    Q_OBJECT
public:
    explicit Slpa100uProtocolCom(QObject *parent = nullptr);
    enum t_protocolSymbol{
        ENQ  = 0x05,
        STX  = 0x02,
        ACK  = 0x06,
        NAK  = 0x15,
        SYNC = 0x16
    };

public:
    static bool checkUri(const QString &uri);

private:
    virtual int executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in);
    char crc(const QByteArray &buf);
};

#endif // SLPA100UPROTOCOLCOM_H
