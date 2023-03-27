#ifndef WEIGHTMANAGER_H
#define WEIGHTMANAGER_H

#include <QObject>
#include "wm100.h"

class WeightManager : public QObject
{
    Q_OBJECT

public:
    explicit WeightManager(QObject*);
    void start();
    void stop();

private:
    Wm100* wm100 = nullptr;
    bool started = false;

signals:
    void log(const int, const QString&);
    void showMessageBox(const QString&, const QString&);
    void weightParamChanged(const int, const double, const bool);

public slots:
    void onSettingsChanged();
    void onSetWeightParam(const int);
    void onWeightStatusChanged(channel_status&);
    void onErrorStatusChanged(int);
};

#endif // WEIGHTMANAGER_H
