#ifndef WM100PROTOCOLDEMO_H
#define WM100PROTOCOLDEMO_H

#include <QObject>
#include "Wm100Protocol.h"

class Wm100ProtocolDemo : public Wm100Protocol
{
    Q_OBJECT
public:
    explicit Wm100ProtocolDemo(QObject *parent = nullptr);
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

public:
    static bool checkUri(const QString &uri);
    virtual int open(const QString &uri);
    virtual void close();
    virtual bool connected();
    virtual int cGetStatus(channel_status *status);
    virtual int cSetMode(uint8_t mode);
    virtual int cGetMode(uint8_t *mode);
    virtual int cSetZero();
    virtual int cSetTare();
    virtual int cSetTareValue(const double_t tare);
    virtual int cCalibWritePoint(const uint8_t num, const double_t weight) { return 123; }
    virtual int cCalibReadPoint(const uint8_t num, double_t *weight) { return 123; }
    virtual int cCalibStart() { return 123; }
    virtual int cCalibStop() { return 123; }
    virtual int cCalibStatus(calib_status *status) { return 123; }
    virtual int cCalibAccStart() { return 123; }
    virtual int getDeviceMetrics() { return 0; }
    virtual int getChannelParam() { return 0; }
    virtual int resetDevice() { return -19; }
    virtual int getADC(uint32_t *ADCValue) { return 123; }

private:
    virtual int executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in) { return 0; };
    virtual void timerEvent(QTimerEvent *event);

    demo_status demo;
    void initDemo();
};

#endif // WM100PROTOCOLDEMO_H
