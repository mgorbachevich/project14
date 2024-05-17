#ifndef SLPA100UPROTOCOLHTTP_H
#define SLPA100UPROTOCOLHTTP_H

#include <QObject>
#include "Slpa100uProtocol.h"

class Slpa100uProtocolHttp : public Slpa100uProtocol
{
    Q_OBJECT
public:
    explicit Slpa100uProtocolHttp(QObject *parent = nullptr);

public:
    static bool checkUri(const QString &uri);

private:
    uint16_t linesToPrint = 0;
    QByteArray buffer;
    virtual int executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in);
    int readAnswer(prncommand cmd, const QByteArray &out, QByteArray &in);
    int sendCommand(prncommand cmd, const QByteArray &out, QByteArray &in, uint32_t toRead);
    int printBuffer(QByteArray &in);
    int flushBuffer(const QByteArray &out, uint16_t offset, uint16_t flags);
    void parseReply(const QByteArray &reply, QByteArray &answer);
    uint16_t getCRC16(const QByteArray &src);
    int sendImage(const QByteArray &out, QByteArray &in);
};

#endif // SLPA100UPROTOCOLHTTP_H
