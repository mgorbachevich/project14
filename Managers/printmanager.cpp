#include <QDebug>
#include "printmanager.h"

PrintManager::PrintManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ PrintManager::PrintManager";
    slpa = new Slpa100u(this);
}

void PrintManager::start()
{
    qDebug() << "@@@@@ PrintManager::start";
    if (slpa != nullptr && !started)
    {
        int error = slpa->connectDevice("demo://COM3?baudrate=115200&timeout=100");
        slpa->startPolling(200);
        started = (error == 0);
        qDebug() << "@@@@@ PrintManager::start error = " << error;
    }
}

void PrintManager::stop()
{
    qDebug() << "@@@@@ PrintManager::stop";
    if (slpa != nullptr && started)
    {
        slpa->stopPolling();
        int error = slpa->disconnectDevice();
        started = false;
        qDebug() << "@@@@@ PrintManager::stop error = " << error;
    }
}

void PrintManager::onPrint()
{
    qDebug() << "@@@@@ PrintManager::onPrint";
    if(started)
    {
        int error = slpa->printTest(100);
        if(error != 0)
            emit showMessageBox("Сообщение", QString("Ошибка печати %1").arg(error), true);
    }
    emit printed();
}

void PrintManager::onSettingsChanged()
{
    qDebug() << "@@@@@ PrintManager::onSettingsChanged";
    start();
}

