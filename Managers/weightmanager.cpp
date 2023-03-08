#include <QDebug>
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
    weight +=  0.001 * (arc4random() % 10);
    emit weightChanged(weight);
#endif
}


