#ifndef SLPA100UPROTOCOLUSB_H
#define SLPA100UPROTOCOLUSB_H

#include <QObject>
#include "Slpa100uProtocol.h"

class Slpa100uProtocolUsb : public Slpa100uProtocol
{
    Q_OBJECT
public:
    explicit Slpa100uProtocolUsb(QObject *parent = nullptr);

public:
    static bool checkUri(const QString &uri);

private:
    virtual int executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in);
};

#endif // SLPA100UPROTOCOLUSB_H
