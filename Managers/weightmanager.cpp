#include <QDebug>
#include <QDateTime>
#include "weightmanager.h"

WeightManager::WeightManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ WeightManager::WeightManager";

#ifdef WEIGHT_EMULATION
    weight = 5.000;
    this->startTimer(1000);
#endif
}

void WeightManager::timerEvent(QTimerEvent*)
{
#ifdef DEBUG_LOG_BACKGROUND_THREADS
    qDebug() << "@@@@@ WeightManager::timerEvent";
#endif

#ifdef WEIGHT_EMULATION
    weight +=  0.001 * (QDateTime::currentMSecsSinceEpoch() % 10); // Псевдослучайное число
    emit weightChanged(weight);
#endif
}

void WeightManager::onSetWeightParam(const int param, const bool value) // param: 0 - >0<, 1 - >T<, 2 - Auto
{
    emit weightParamChanged(param, value);
}


