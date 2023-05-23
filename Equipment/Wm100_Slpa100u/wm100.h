#ifndef WM100_H
#define WM100_H

#include <QObject>
#include "IO/iobase.h"


class Wm100 : public QObject
{
    Q_OBJECT
public:
    explicit Wm100(QObject *parent = nullptr);

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
    struct demo_status {
        bool active;
        demo_state state;
        int stepZero;
        int stepWaitFix;
        int stepFix;
        int stepWaitZero;
        double weightFix;
        int timerid;
        channel_status status;
        channel_specs  spec;
        device_metrics metrics;
    };
    enum t_command {
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
    enum t_protocolSymbol {
        ENQ = 0x05,
        STX = 0x02,
        ACK = 0x06,
        NAK = 0x15
    };
    enum t_mode{
        modeNormal = 0x00,
        modeCalibration = 0x01,
    };

    int connectDevice(const QString &uri);
    int disconnectDevice();
    int getStatus(channel_status *status);
    int setMode(uint8_t mode);
    int getMode(uint8_t *mode);
    int setZero();
    int setTare();
    int setTareValue(const double_t tare);
    int calibWritePoint(const uint8_t num, const double_t weight);
    int calibReadPoint(const uint8_t num, double_t *weight);
    int calibStart();
    int calibStop();
    int calibStatus(calib_status *status);
    int calibAccStart();

    void startPolling(int time);
    void stopPolling();
    bool isConnected();
    QString errorDescription(const int err) const;

protected:
    int getDeviceMetrics();
    int getChannelParam();
    int resetDevice();
    int getADC(uint32_t *ADCValue);

protected:
    IoBase *io = nullptr;
    int command(t_command cmd, const QByteArray &out, QByteArray &in);

private:
    demo_status demo;
    int timerid = 0;
    int timerInterval = 0;
    int lastStatusError = 0;
    channel_status lastStatus = {0, 0.0, 0.0, 0};
    device_metrics deviceMetrics;
    channel_specs channelParams;
    char crc(const QByteArray &buf);
    bool checkForDemo(const QString &uri);
    void initDemo();
    void startDemo();
    void stopDemo();

    virtual void timerEvent(QTimerEvent *event);

signals:
    void weightStatusChanged(channel_status&);
    void errorStatusChanged(int);
    void pollingStatus(int);
};

#endif // WM100_H
