#ifndef WEIGHTMANAGER_H
#define WEIGHTMANAGER_H

#include <QObject>
#include "wm100.h"
#include "constants.h"

class WeightManager : public QObject
{
    Q_OBJECT

public:
    explicit WeightManager(QObject*);
    int start(const QString&, const bool);
    void stop();
    QString version() const;
    void setWeightParam(const int);
    double getWeight() const { return status.weight; }
    double getTare() const { return status.tare; }
    QString getErrorDescription(const int) const;
    bool isError() const { return errorCode != 0 || isStateError(status); }
    bool isWeightFixed() const { return isFlag(status, 0); }
    bool isZeroFlag() const { return isFlag(status, 1); }
    bool isTareFlag() const { return isFlag(status, 3); }
    bool isDemoMode() const { return demo || DEMO_ONLY; }

    bool setSystemDateTime = false;

private:
    bool isFlag(Wm100Protocol::channel_status, int) const;
    bool isStateError(Wm100Protocol::channel_status) const;

    Wm100* wm100 = nullptr;
    bool started = false;
    int errorCode = 0;
    Wm100Protocol::channel_status status = {0, 0.0, 0.0, 0};
    bool demo = false;

signals:
    void paramChanged(const int, const int);

public slots:
    void onStatusChanged(Wm100Protocol::channel_status&);
    void onErrorStatusChanged(int);
};

#endif // WEIGHTMANAGER_H
