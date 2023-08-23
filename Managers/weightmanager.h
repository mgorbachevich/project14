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
    QString version();
    bool isError() { return errorCode != 0 || isStateError(status); }
    void setWeightParam(const int);
    double getWeight() { return status.weight; }
    int getPieces() { return pieces; }
    double getTare() { return status.tare; }
    void setPieces(const int v) { pieces = v; }
    bool isWeightFixed() { return isFlag(status, 0); }
    bool isZeroFlag() { return isFlag(status, 1); }
    bool isTareFlag() { return isFlag(status, 3); }

private:
    bool isFlag(Wm100::channel_status, int);
    bool isStateError(Wm100::channel_status);

    Wm100* wm100 = nullptr;
    bool started = false;
    int pieces = 1;
    int errorCode = 0;
    Wm100::channel_status status = {0, 0.0, 0.0, 0};

signals:
    void paramChanged(const int, const int, const QString&);

public slots:
    void onStatusChanged(Wm100::channel_status&);
    void onErrorStatusChanged(int);
};

#endif // WEIGHTMANAGER_H
