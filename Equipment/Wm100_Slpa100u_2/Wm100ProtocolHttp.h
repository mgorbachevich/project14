#ifndef WM100PROTOCOLHTTP_H
#define WM100PROTOCOLHTTP_H

#include <QObject>
#include "Wm100Protocol.h"

class Wm100ProtocolHttp : public Wm100Protocol
{
    Q_OBJECT
public:
    explicit Wm100ProtocolHttp(QObject *parent = nullptr);

public:
    static bool checkUri(const QString &uri);
    virtual int open(const QString &uri);
    virtual int cSetDateTime(const QDateTime &datetime, const QString &uri);
    virtual int cDeamonVersion(QString &version, QString &build, const QString &uri);

private:
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in);
    int readAnswer(QByteArray &in, uint32_t toRead);
    int sendCommand(wmcommand cmd, const QByteArray &out, QByteArray &in, uint8_t inf);
    void parseReply(const QByteArray &reply, QByteArray &answer);
    uint16_t getCRC16(const QByteArray &src);
    bool checkCRC(const QByteArray &in);
};

#endif // WM100PROTOCOLHTTP_H
