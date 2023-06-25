#ifndef WEIGHTMANAGER_H
#define WEIGHTMANAGER_H

#include <QObject>
#include "wm100.h"
#include "constants.h"

#define WEIGHT_STATE_ERROR_MASK 0x0003FE00

class WeightManager : public QObject
{
    Q_OBJECT

public:
    explicit WeightManager(QObject*);
    void start();
    void stop();
    QString version();
    bool isError() { return error != 0 || isStateError(status); }
    void setWeightParam(const int);
    double getWeight() { return status.weight; }
    int getPieces() { return pieces; }
    double getTare() { return status.tare; }
    void setPieces(const int v) { pieces = v; }
    bool isWeightFixed() { return isFlag(status, 0); }
    bool isZeroFlag() { return isFlag(status, 1); }
    bool isTareFlag() { return isFlag(status, 3); }

private:
    bool isFlag(Wm100::channel_status s, int shift) { return (s.state & (0x00000001 << shift)) != 0; }
    bool isStateError(Wm100::channel_status s) { return (s.state & WEIGHT_STATE_ERROR_MASK) != 0; }
    void onParamChanged(const EquipmentParam, QVariant, const QString&);

    Wm100* wm100 = nullptr;
    bool started = false;
    int pieces = 1;
    int error = 0;
    Wm100::channel_status status = {0, 0.0, 0.0, 0};

signals:
    void paramChanged(const int, const QString&, const QString&);

public slots:
    void onStatusChanged(Wm100::channel_status&);
    void onErrorStatusChanged(int);
};

#endif // WEIGHTMANAGER_H
