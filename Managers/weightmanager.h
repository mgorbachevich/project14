#ifndef WEIGHTMANAGER_H
#define WEIGHTMANAGER_H

#include <QObject>
#include "constants.h"

class WeightManager : public QObject
{
    Q_OBJECT

public:
    explicit WeightManager(QObject*);
    virtual void timerEvent(QTimerEvent*);

private:
#ifdef WEIGHT_EMULATION
    double weight = 0;
#endif

private:
    bool tareWeight = false;
    bool zeroWeight = false;
    bool autoWeight = false;

signals:
    void log(const int, const QString&);
    void showMessageBox(const QString&, const QString&);
    void weightChanged(const double);
    void weightParamChanged(const int, const bool);

public slots:
    void onSetWeightParam(const int, const bool);
};

#endif // WEIGHTMANAGER_H
