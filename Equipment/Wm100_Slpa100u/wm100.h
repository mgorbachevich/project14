#ifndef WM100_H
#define WM100_H

#include <QObject>
#include <QTimer>
#include "Wm100Protocol.h"


#define MOD_VERSION "1.17"


class Wm100 : public QObject
{
    Q_OBJECT
public:
    explicit Wm100(QObject *parent = nullptr);
    ~Wm100();
    static QString getVersionString() { return MOD_VERSION; }

public:
    int connectDevice(const QString &uri);
    int disconnectDevice();
    int getStatus(Wm100Protocol::channel_status *status);
    int getStatusEx(Wm100Protocol::channel_status_ex *status);
    int setMode(uint8_t mode);
    int getMode(uint8_t *mode);
    int setZero();
    int setTare();
    int setTareValue(const double_t tare);
    int calibWritePoint(const uint8_t num, const double_t weight);
    int calibReadPoint(const uint8_t num, double_t *weight);
    int calibStart();
    int calibStop();
    int calibStatus(Wm100Protocol::calib_status *status);
    int calibAccStart();
    int getDeviceMetrics(Wm100Protocol::device_metrics *deviceMetrics);
    int getChannelParam(Wm100Protocol::channel_specs *channelParam);
    int getADC(uint32_t *ADCValue);
    int controllerId(Wm100Protocol::controller_id *id);

    int setDateTime(const QDateTime &datetime);
    int getDaemonVersion(QString &version, QString &build);
    int killDaemon();
    int setDisplayData(const Wm100Protocol::display_data &dd);

    void startPolling(int time);
    void stopPolling();
    bool isConnected();
    QString errorDescription(const int err) const;
    int readConnectParam(const QString &filename, const QString &param, QString &uri);

    Wm100Protocol::deviceinterface getInterface();
    Wm100Protocol::deviceinterface checkUri(const QString &uri);

protected:
    int resetDevice();

private:
    Wm100Protocol *protocol = nullptr;
    QTimer timer;
    int timerid = 0;
    int timerInterval = 0;
    int lastStatusError = 0;
    Wm100Protocol::channel_status lastStatus = {0, 0.0, 0.0, 0};
    Wm100Protocol::device_metrics deviceMetrics;
    Wm100Protocol::channel_specs channelParams;

    virtual void timerEvent(QTimerEvent *event);

signals:
    void weightStatusChanged(Wm100Protocol::channel_status&);
    void errorStatusChanged(int);
    void pollingStatus(int);

private slots:
    void onTimer();
};

#endif // WM100_H
