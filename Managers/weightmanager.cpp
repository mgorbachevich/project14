#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "weightmanager.h"
#include "constants.h"
#include "tools.h"

WeightManager::WeightManager(QObject *parent, const bool demo): QObject(parent)
{
    Tools::debugLog("@@@@@ WeightManager::WeightManager ");
    if(!demo)
    {
        wm100 = new Wm100(this);
        connect(wm100, &Wm100::weightStatusChanged, this, &WeightManager::onStatusChanged);
        connect(wm100, &Wm100::errorStatusChanged, this, &WeightManager::onErrorStatusChanged);
    }
}

int WeightManager::start(const QString& url) // return error
{
    int e = 0;
    if (wm100 != nullptr && !started)
    {
        Tools::debugLog("@@@@@ WeightManager::start " + url);
        e = wm100->connectDevice(url);
        started = (e == 0);
        if(started) wm100->startPolling(200);
        Tools::debugLog("@@@@@ WeightManager::start error " + QString::number(e));
    }
    return e;
}

void WeightManager::stop()
{
    if (wm100 != nullptr && started)
    {
        Tools::debugLog("@@@@@ WeightManager::stop");
        started = false;
        wm100->stopPolling();
        int e = wm100->disconnectDevice();
        Tools::debugLog("@@@@@ WeightManager::stop error " + QString::number(e));
    }
}

QString WeightManager::version() const
{
    if (wm100 != nullptr)
    {
        Wm100Protocol::device_metrics dm;
        if(wm100->getDeviceMetrics(&dm) >= 0) return QString::number(dm.protocol_version);
    }
    return "DEMO";
}

void WeightManager::setWeightParam(const int param)
{
    Tools::debugLog("@@@@@ WeightManager::onSetWeightParam " + QString::number(param));
    if (wm100 == nullptr || !started) return;
    switch (param)
    {
    case EquipmentParam_Tare:
        wm100->setTare();
        break;
    case EquipmentParam_Zero:
        wm100->setZero();
        wm100->setTare();
        break;
    default:
        break;
    }
}

bool WeightManager::isFlag(Wm100Protocol::channel_status s, int shift) const
{
    return (s.state & (0x00000001 << shift)) != 0;
}

bool WeightManager::isStateError(Wm100Protocol::channel_status s) const
{
    return isFlag(s, 5) || isFlag(s, 6) || isFlag(s, 7) || isFlag(s, 8) || isFlag(s, 9);
}

QString WeightManager::getErrorDescription(const int e) const
{
    switch(e)
    {
    case 0:    return "Ошибок нет";
    case 5003: return "Ошибка автонуля при включении";
    case 5004: return "Перегрузка по весу";
    case 5005: return "Ошибка при получении измерения";
    case 5006: return "Весы недогружены";
    case 5007: return "Ошибка: нет ответа от АЦП";
    //default: return "Неизвестная ошибка";
    }
    return wm100 == nullptr ? "Весовой модуль не подключен" : wm100->errorDescription(e);
}

void WeightManager::onStatusChanged(Wm100Protocol::channel_status &s)
{
    Tools::debugLog(QString("@@@@@ WeightManager::onStatusChanged state=%1b weight=%2 tare=%3").arg(
        QString::number(s.state, 2), QString::number(s.weight), QString::number(s.tare)));

    //bool b0 = isFlag(s, 0); // признак фиксации веса
    //bool b1 = isFlag(s, 1); // признак работы автонуляmain()
    //bool b2 = isFlag(s, 2); // "0"- канал выключен, "1"- канал включен
    //bool b3 = isFlag(s, 3); // признак тары
    //bool b4 = isFlag(s, 4); // признак успокоения веса
    bool b5 = isFlag(s, 5); // ошибка автонуля при включении
    bool b6 = isFlag(s, 6); // перегрузка по весу
    bool b7 = isFlag(s, 7); // ошибка при получении измерения (нет градуировки весов или она не правильная)
    bool b8 = isFlag(s, 8); // весы недогружены
    bool b9 = isFlag(s, 9); // ошибка: нет ответа от АЦП

    EquipmentParam param = EquipmentParam_WeightValue;
    int e = 0;
    if(b5 || b6 || b7 || b8 || b9) // Ошибка состояния
    {
        param = EquipmentParam_WeightError;
        if(b5 && isFlag(status, 5) != b5) e = 5003;
        if(b6 && isFlag(status, 6) != b6) e = 5004;
        if(b7 && isFlag(status, 7) != b7) e = 5005;
        if(b8 && isFlag(status, 8) != b8) e = 5006;
        if(b9 && isFlag(status, 9) != b9) e = 5007;
    }
    else if(isStateError(status) && errorCode == 0) param = EquipmentParam_WeightError; // Ошибка исчезла

    status.weight = s.weight;
    status.tare = s.tare;
    status.state = s.state;
    emit paramChanged(param, e);
}

void WeightManager::onErrorStatusChanged(int e)
{
    Tools::debugLog("@@@@@ WeightManager::onErrorStatusChanged " + QString::number(e));
    if(errorCode != e)
    {
        errorCode = e;
        emit paramChanged(EquipmentParam_WeightError, e);
    }
}



