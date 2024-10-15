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
    ~Wm100Protocol();

public:
    struct channel_status {
        uint16_t state;
        double_t weight;
        double_t tare;
        uint8_t  flags;
    };

    struct channel_status_ex {
        uint16_t state;
        double_t weight;
        double_t tare;
        uint8_t  flags;
        double_t weightprecise;
        uint8_t  levelx;
        uint8_t  levely;
    };

    struct channel_specs {
        uint16_t flags;
        uint8_t  dec_point;
        int8_t   measure;
        double_t max;
        double_t min;
        double_t tare;
        double_t ranges[3];
        double_t discreteness[4];
        uint8_t  calibration_points_number;
        uint8_t  calibrations_number;
    };

    struct calib_status {
        uint8_t channel_number;
        double_t reference_point;
        uint8_t  reference_point_state;
    };

    struct controller_id {
        QByteArray id;
        uint16_t crc16;
        QString  firmware;
    };

    struct device_metrics {
        uint8_t  type;
        uint8_t  subtype;
        uint16_t protocol_version;
        uint8_t  model;
        uint8_t  lang;
        QString  name;
    };

    struct display_data
    {
        QString weight;
        QString price;
        QString cost;
        QString tare;
        QString text;
        bool flZero;
        bool flTare;
        bool flCalm;
        bool flAuto;
        bool flMemory;
        bool flUprow;
        bool flPieces;
        bool flTools;
        bool flDataExchange;
        bool flPencil;
        bool flLock;
        bool flPerKg;
        bool flPer100g;
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
        cmGetStatusEx = 0x3b,
        cmCalibWritePoint = 0x70,
        cmCalibReadPoint = 0x71,
        cmCalibStart = 0x72,
        cmCalibStatus = 0x73,
        cmCalibStop = 0x74,
        cmGetADC = 0x75,
        cmCalibAccStart = 0x76,
        cmGetChannelParam = 0xe8,
        cmReset = 0xf0,
        cmControllerID = 0xf4,
        cmGetDeviceMetrics = 0xfc,
    };
    enum t_mode{
        modeNormal = 0x00,
        modeCalibration = 0x01,
    };
    enum deviceinterface {
        diNone,
        diCom,
        diHttp,
        diUsb,
        diDemo
    };


public:
    virtual int open(const QString &uri) = 0;
    virtual void close();
    virtual bool connected();
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
    virtual int cSetDateTime(const QDateTime &datetime, const QString &uri);
    virtual int cDaemonVersion(QString &version, QString &build, const QString &uri);
    virtual int cKillDaemon(const QString &uri);
    virtual int cDisplayData(const display_data &dd, const QString &uri);
    virtual void getDeviceMetrics(device_metrics *metrics);
    virtual void getChannelParam(channel_specs *params);
    deviceinterface getInterface();

protected:
    IoBase *io = nullptr;
    deviceinterface deviceInterface = diNone;
    channel_status lastStatus = {0, 0.0, 0.0, 0};
    device_metrics deviceMetrics;
    channel_specs channelParams;

private:
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in) = 0;

signals:

};

#endif // WM100PROTOCOL_H
