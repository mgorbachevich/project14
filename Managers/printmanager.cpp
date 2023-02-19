#include <QDebug>
#include "printmanager.h"

PrintManager::PrintManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ PrintManager::PrintManager";
}

void PrintManager::onPrint()
{
    qDebug() << "@@@@@ PrintManager::onPrint";
    emit showMessageBox("Сообщение", "Напечатано!");
    emit printed();
}
