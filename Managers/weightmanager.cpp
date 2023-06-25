#include <QDebug>
#include <QDateTime>
#include "weightmanager.h"

WeightManager::WeightManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ WeightManager::WeightManager";
    wm100 = new Wm100(this);
    connect(wm100, &Wm100::weightStatusChanged, this, &WeightManager::onStatusChanged);
    connect(wm100, &Wm100::errorStatusChanged, this, &WeightManager::onErrorStatusChanged);
}

void WeightManager::start()
{
    qDebug() << "@@@@@ WeightManager::start";
    if (wm100 != nullptr && !started)
    {
        int e = wm100->connectDevice("demo://COM3?baudrate=115200&timeout=100");
        wm100->startPolling(200);
        started = (e == 0);
        qDebug() << "@@@@@ WeightManager::start error = " << e;
    }
}

void WeightManager::stop()
{
    qDebug() << "@@@@@ WeightManager::stop";
    if (wm100 != nullptr && started)
    {
        wm100->stopPolling();
        int e = wm100->disconnectDevice();
        started = false;
        qDebug() << "@@@@@ WeightManager::stop error = " << e;
    }
}

QString WeightManager::version()
{
    return "?";
}

void WeightManager::setWeightParam(const int param)
{
    qDebug() << "@@@@@ WeightManager::onSetWeightParam " << param;
    if (wm100 == nullptr || !started) return;
    switch (param)
    {
    case EquipmentParam_TareFlag:
        wm100->setTare();
        break;
    case EquipmentParam_ZeroFlag:
        wm100->setZero();
        break;
    default:
        break;
    }
}

void WeightManager::onParamChanged(const EquipmentParam p, QVariant v, const QString& description)
{
    emit paramChanged(p, v.toString(), description);
}

void WeightManager::onStatusChanged(Wm100::channel_status &s)
{
    bool b0 = isFlag(s, 0);
    bool b1 = isFlag(s, 1);
    bool b3 = isFlag(s, 3);
    bool b5 = isFlag(s, 5);
    bool b6 = isFlag(s, 6);
    bool b7 = isFlag(s, 7);
    bool b8 = isFlag(s, 8);
    bool b9 = isFlag(s, 9);
    if(status.weight != s.weight) onParamChanged(EquipmentParam_WeightValue, s.weight, "");
    if(status.tare != s.tare)     onParamChanged(EquipmentParam_TareValue, s.tare, "");
    if(isWeightFixed() != b0)     onParamChanged(EquipmentParam_WeightFixed, b0, "");
    if(isZeroFlag() != b1)        onParamChanged(EquipmentParam_ZeroFlag, b1, "");
    if(isTareFlag() != b3)        onParamChanged(EquipmentParam_TareFlag, b3, "");
    if(isFlag(status, 5) != b5 && b5) onParamChanged(EquipmentParam_Error, 5003, "Ошибка автонуля при включении");
    if(isFlag(status, 6) != b6 && b6) onParamChanged(EquipmentParam_Error, 5004, "Перегрузка по весу");
    if(isFlag(status, 7) != b7 && b7) onParamChanged(EquipmentParam_Error, 5005, "Ошибка при получении измерения");
    if(isFlag(status, 8) != b8 && b8) onParamChanged(EquipmentParam_Error, 5006, "Весы недогружены");
    if(isFlag(status, 9) != b9 && b9) onParamChanged(EquipmentParam_Error, 5007, "Ошибка: нет ответа от АЦП");

    if((isStateError(status) != isStateError(s)) && !isStateError(s) && (error == 0))
        onParamChanged(EquipmentParam_Error, 0, "Ошибок нет");

    status.weight = s.weight;
    status.tare = s.tare;
    status.state = s.state;
}

void WeightManager::onErrorStatusChanged(int errorCode)
{
    //qDebug() << "@@@@@ WeightManager::onErrorStatusChanged ";
    if(error != errorCode)
    {
        if(errorCode != 0)
            onParamChanged(EquipmentParam_Error, error, wm100->errorDescription(error));
        else if(!isStateError(status))
            onParamChanged(EquipmentParam_Error, 0, "Ошибок нет");
        error = errorCode;
    }
}



