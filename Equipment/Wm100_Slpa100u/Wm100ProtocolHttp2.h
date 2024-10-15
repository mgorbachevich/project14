#ifndef WM100PROTOCOLHTTP2_H
#define WM100PROTOCOLHTTP2_H

#include <QObject>
//#include <QThread>
#include "Wm100Protocol.h"
//#include "../ExchangeThread.h"

class Wm100ProtocolHttp2 : public Wm100Protocol
{
    Q_OBJECT
public:
    explicit Wm100ProtocolHttp2(QObject *parent = nullptr);

public:
    static bool checkUri(const QString &uri);
    virtual int open(const QString &uri);
    virtual int cSetDateTime(const QDateTime &datetime, const QString &uri);
    virtual int cDaemonVersion(QString &version, QString &build, const QString &uri);
    virtual int cKillDaemon(const QString &uri);
    virtual int cDisplayData(const display_data &dd, const QString &uri);

    virtual int cGetStatus(channel_status *status);
    virtual int cGetStatusEx(channel_status_ex *status);
    virtual int cSetMode(uint8_t mode);
    virtual int cGetMode(uint8_t *mode);
    virtual int cSetZero();
    virtual int cSetTare();
    virtual int cSetTareValue(const double_t tare);
    virtual int cCalibWritePoint(const uint8_t num, const double_t weight);
    virtual int cCalibReadPoint(const uint8_t num, double_t *weight);
    virtual int cCalibStart();
    virtual int cCalibStop();
    virtual int cCalibStatus(calib_status *status);
    virtual int cCalibAccStart();
    virtual int cControllerId(controller_id *id);
    virtual int cGetDeviceMetrics();
    virtual int cGetChannelParam();
    virtual int cResetDevice();
    virtual int cGetADC(uint32_t *ADCValue);
private:
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in);
    int executeCommandGet(const QString cmd, QByteArray &in);
    int executeCommandPost(const QString cmd, const QString val = "");
    int readAnswer(QByteArray &in, uint32_t toRead);
    int sendCommand(wmcommand cmd, const QByteArray &out, QByteArray &in, uint8_t inf);
    int parseReply(const QByteArray &reply, QByteArray &answer);
    int parseError(const QByteArray &reply);
    int parseCommandError(const QByteArray &reply);
    int parseParamInt(const QByteArray &reply, const QString param);
};

#endif // WM100PROTOCOLHTTP2_H
