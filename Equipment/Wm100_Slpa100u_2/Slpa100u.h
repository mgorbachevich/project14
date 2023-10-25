#ifndef SLPA100U_H
#define SLPA100U_H

#include <QObject>
#include <QImage>
#include "Slpa100uProtocol.h"

class Slpa100u : public QObject
{
    Q_OBJECT
public:
    explicit Slpa100u(QObject *parent = nullptr);

public:
    int connectDevice(const QString &uri);
    int disconnectDevice();
    void startPolling(int time);
    void stopPolling();
    bool isConnected();
    QString errorDescription(const int err) const;

    int print(QImage &p);
    int printTest(int lines);
    int feed();
    int getStatus(Slpa100uProtocol::prnanswer *status);
    int reset();
    int setBrightness(int value);
    int setOffset(int value);
    int setPaper(Slpa100uProtocol::papertype value);
    int setSensor(bool value);
    int getOffset(int *value);
    int getPrinterVersion();

private:
    Slpa100uProtocol *protocol = nullptr;
    int timerid = 0;
    int timerInterval = 0;
    int lastStatusError = 0;
    uint16_t lastStatus = 0;
    virtual void timerEvent(QTimerEvent *event);

signals:
    void printerStatusChanged(uint16_t);
    void printerErrorChanged(int);
    void pollingStatus(int);
};

#endif // SLPA100U_H
