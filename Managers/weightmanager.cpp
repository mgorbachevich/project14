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

int WeightManager::start(const QString& url, const bool) // return error
{
    qDebug() << "@@@@@ WeightManager::start url = " << url;
    int error = 0;
    if (wm100 != nullptr && !started)
    {
        error = wm100->connectDevice(url);
        qDebug() << "@@@@@ WeightManager::start error = " << error;
        started = (error == 0);
        if(started) wm100->startPolling(200);
    }
    return error;
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

bool WeightManager::isFlag(Wm100::channel_status s, int shift)
{
    return (s.state & (0x00000001 << shift)) != 0;
}

bool WeightManager::isStateError(Wm100::channel_status s)
{
    return isFlag(s, 5) || isFlag(s, 6) || isFlag(s, 7) || isFlag(s, 8) || isFlag(s, 9);
}

void WeightManager::onStatusChanged(Wm100::channel_status &s)
{
    qDebug() << QString("@@@@@ WeightManager::onStatusChanged state=%1b weight=%2 tare=%3").arg(
                QString::number(s.state, 2),
                QString::number(s.weight),
                QString::number(s.tare));

    //bool b0 = isFlag(s, 0); // признак фиксации веса
    //bool b1 = isFlag(s, 1); // признак работы автонуля
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
    QString description = "Ошибок нет";
    if(isStateError(s))
    {
        param = EquipmentParam_WeightError;
        if(b5 && isFlag(status, 5) != b5)
        {
            e = 5003;
            description = "Ошибка автонуля при включении";
        }
        if(b6 && isFlag(status, 6) != b6)
        {
            e = 5004;
            description = "Перегрузка по весу";
        }
        if(b7 && isFlag(status, 7) != b7)
        {
            e = 5005;
            description = "Ошибка при получении измерения";
        }
        if(b8 && isFlag(status, 8) != b8)
        {
            e = 5006;
            description = "Весы недогружены";
        }
        if(b9 && isFlag(status, 9) != b9)
        {
            e = 5007;
            description = "Ошибка: нет ответа от АЦП";
        }
    }
    else
        if(isStateError(status) && error == 0)
            param = EquipmentParam_WeightError;

    status.weight = s.weight;
    status.tare = s.tare;
    status.state = s.state;
    emit paramChanged(param, e, description);
}

void WeightManager::onErrorStatusChanged(int errorCode)
{
    if(error != errorCode)
    {
        error = errorCode;
        QString description = error == 0? "Ошибок нет" : wm100->errorDescription(error);
        emit paramChanged(EquipmentParam_WeightError, error, description);
    }
}



