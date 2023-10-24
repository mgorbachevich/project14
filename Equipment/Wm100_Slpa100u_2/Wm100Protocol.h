#ifndef WM100PROTOCOL_H
#define WM100PROTOCOL_H

#include "IO/iobase.h"
#include <QObject>
#include <QMutex>

class Wm100Protocol : public QObject
{
    Q_OBJECT
public:
    explicit Wm100Protocol(QObject *parent = nullptr);

public:
    struct channel_status {
        uint16_t state;
        double_t weight;
        double_t tare;
        uint8_t  flags;
    };

    struct channel_specs {
        uint16_t flags;
        uint8_t  reserved0;
        int8_t   measure;
        double_t max;
        double_t min;
        double_t tare;
        double_t ranges[3];
        double_t discreteness[4];
        uint8_t  calibration_points_number;
        uint8_t  reserved1;
    };

    struct calib_status {
        uint8_t channel_number;
        double_t reference_point;
        uint8_t  reference_point_state;
    };

    struct device_metrics {
        uint8_t  type;
        uint8_t  subtype;
        uint16_t protocol_version;
        uint8_t  model;
        uint8_t  lang;
        QString  name;
    };
    enum demo_state {
        dmNone = 0,
        dmZero = 1,
        dmWaitFix = 2,
        dmFix = 3,
        dmWaitZero = 4
    };

    enum wmcommand {
        cmSetMode = 0x07,
        cmGetMode = 0x12,
        cmSetExchangeParam = 0x14,
        cmSetZero = 0x30,
        cmSetTare = 0x31,
        cmSetTareValue = 0x32,
        cmGetStatus = 0x3a,
        cmCalibWritePoint = 0x70,
        cmCalibReadPoint = 0x71,
        cmCalibStart = 0x72,
        cmCalibStatus = 0x73,
        cmCalibStop = 0x74,
        cmGetADC = 0x75,
        cmCalibAccStart = 0x76,
        cmGetChannelParam = 0xe8,
        cmReset = 0xf0,
        cmGetDeviceMetrics = 0xfc,
    } ;
    enum t_mode{
        modeNormal = 0x00,
        modeCalibration = 0x01,
    };

public:
    virtual int open(const QString &uri) = 0;
    virtual void close();
    virtual bool connected();
    virtual int cGetStatus(channel_status *status);
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
    virtual int cGetDeviceMetrics();
    virtual int cGetChannelParam();
    virtual int cResetDevice();
    virtual int cGetADC(uint32_t *ADCValue);
    void getDeviceMetrics(device_metrics *metrics);
    void getChannelParam(channel_specs *params);

protected:
    IoBase *io = nullptr;
    QMutex mtx;

private:
    channel_status lastStatus = {0, 0.0, 0.0, 0};
    device_metrics deviceMetrics;
    channel_specs channelParams;
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in) = 0;
//    int command(prncommand cmd, const QByteArray &out, QByteArray &in);
//    int command(prncommand cmd, const QByteArray &out, prnanswer *ans);
//    int command(prncommand cmd, const QByteArray &out);
//    int command(prncommand cmd, prnanswer *ans = nullptr);
//    int command(prncommand cmd, memorytest *ans);
//    int command(prncommand cmd, counters *ans);

signals:

};

#endif // WM100PROTOCOL_H
