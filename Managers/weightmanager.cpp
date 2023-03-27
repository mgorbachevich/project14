#include <QDebug>
#include <QDateTime>
#include "appmanager.h"
#include "weightmanager.h"

WeightManager::WeightManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ WeightManager::WeightManager";
    wm100 = new Wm100(this);
    connect(wm100, &Wm100::weightStatusChanged, this, &WeightManager::onWeightStatusChanged);
    connect(wm100, &Wm100::errorStatusChanged, this, &WeightManager::onErrorStatusChanged);
}

void WeightManager::start()
{
    qDebug() << "@@@@@ WeightManager::start";
    if (wm100 != nullptr && !started)
    {
        int error = wm100->connectDevice("demo://COM3?baudrate=115200&timeout=100");
        wm100->startPolling(200);
        started = (error == 0);
        qDebug() << "@@@@@ WeightManager::start error = " << error;
    }
}

void WeightManager::stop()
{
    qDebug() << "@@@@@ WeightManager::stop";
    if (wm100 != nullptr && started)
    {
        wm100->stopPolling();
        int error = wm100->disconnectDevice();
        started = false;
        qDebug() << "@@@@@ WeightManager::stop error = " << error;
    }
}

void WeightManager::onSettingsChanged()
{
    qDebug() << "@@@@@ WeightManager::onSettingsChanged";
    start();
}

void WeightManager::onSetWeightParam(const int param)
{
    if (wm100 != nullptr && started)
    {
        switch (param)
        {
        case AppManager::WeightParam_TareFlag:
            wm100->setTare();
            break;
        case AppManager::WeightParam_ZeroFlag:
            wm100->setZero();
            break;
        default:
            break;
        }
    }
}

void WeightManager::onWeightStatusChanged(channel_status &status)
{
    emit weightParamChanged(AppManager::WeightParam_WeightValue, status.weight, true);
    emit weightParamChanged(AppManager::WeightParam_TareValue, status.tare, true);
    emit weightParamChanged(AppManager::WeightParam_ZeroFlag, 0, (status.state & 0x0002) != 0); // бит 1 — признак работы автонуля
    emit weightParamChanged(AppManager::WeightParam_TareFlag, 0, (status.state & 0x0008) != 0); // бит 3 — признак тары
}

void WeightManager::onErrorStatusChanged(int status)
{

}


