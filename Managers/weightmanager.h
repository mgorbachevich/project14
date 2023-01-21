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

signals:
    void showMessageBox(const QString&, const QString&);
    void weightChanged(const double&);
};

#endif // WEIGHTMANAGER_H
