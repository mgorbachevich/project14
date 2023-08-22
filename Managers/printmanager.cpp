#include <QDebug>
#include "printmanager.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productdbtable.h"
#include "tools.h"
#include "database.h"

PrintManager::PrintManager(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ PrintManager::PrintManager";
    slpa = new Slpa100u(this);
    connect(slpa, &Slpa100u::printerErrorChanged, this, &PrintManager::onErrorStatusChanged);
    connect(slpa, &Slpa100u::printerStatusChanged, this, &PrintManager::onStatusChanged);
}

int PrintManager::start(const QString& url, const bool demo)
{
    qDebug() << "@@@@@ PrintManager::start";
    demoMode = demo;
    int error = 0;
    if (slpa != nullptr && !started)
    {
        error = slpa->connectDevice(url);
        started = (error == 0);
        if(started) slpa->startPolling(200);
        qDebug() << "@@@@@ PrintManager::start error = " << error;
    }
    return error;
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

QString PrintManager::version()
{
    return "?";
}

void PrintManager::print(DataBase* db, const DBRecord& user, const DBRecord& product, const QString& quantity, const QString& price, const QString& amount)
{
    qDebug() << "@@@@@ PrintManager::onPrint";
    if(!started) return;
    int e = slpa->printTest(100);
    if(e == 0)
    {
        TransactionDBTable* t = (TransactionDBTable*)db->getTableByName(DBTABLENAME_TRANSACTIONS);
        DBRecord r = t->createRecord(
                user[UserDBTable::Code].toInt(),
                Tools::stringToInt(product[ProductDBTable::Code]),
                0, // todo номер этикетки
                Tools::stringToDouble(quantity),
                Tools::stringToInt(price),
                Tools::stringToInt(amount));
        emit printed(r);
    }
    else
    {
        //QString s = QString("Ошибка печати %1").arg(error);
        //emit showMessageBox("Внимание!", s, true);
    }
}

void PrintManager::feed()
{
    qDebug() << "@@@@@ PrintManager::feed";
    if(!started) return;
    slpa->feed();
    // int e = slpa->feed(); todo
}

bool PrintManager::isStateError(uint16_t s)
{
    if(demoMode) return false;
    return isFlag(s, 0) || (isFlag(s, 1) && isFlag(s, 6)) || isFlag(s, 2) || isFlag(s, 3) || isFlag(s, 8);
}

void PrintManager::onStatusChanged(uint16_t s)
{
    if(demoMode)
    {
        status = 0;
        return;
    }
    bool b0 = isFlag(s, 0);
    bool b1 = isFlag(s, 1);
    bool b2 = isFlag(s, 2);
    bool b3 = isFlag(s, 3);
    bool b6 = isFlag(s, 6);
    bool b8 = isFlag(s, 8);

    if(isFlag(status, 0) != b0 && b0)
        emit paramChanged(EquipmentParam_PrintError, 1003, "Нет бумаги! Установите новый рулон!");
    if(isFlag(status, 1) != b1 && b1 && b6)
        emit paramChanged(EquipmentParam_PrintError, 1005, "Снимите этикетку!");
    if(isFlag(status, 2) != b2 && !b2)
        emit paramChanged(EquipmentParam_PrintError, 1006, "Этикетка не спозиционирована! Нажмите клавишу промотки!");
    if(isFlag(status, 3) != b3 && b3)
        emit paramChanged(EquipmentParam_PrintError, 1004, "Закройте головку принтера!");
    if(isFlag(status, 8) != b8 && b8)
        emit paramChanged(EquipmentParam_PrintError, 1008, "Ошибка памяти принтера!");
    if((isStateError(status) != isStateError(s)) && !isStateError(s) && (error == 0))
        emit paramChanged(EquipmentParam_PrintError, 0, "");
    status = s;
}

void PrintManager::onErrorStatusChanged(int errorCode)
{
    if(demoMode)
    {
        error = 0;
        return;
    }
    if(error != errorCode)
    {
        if(errorCode != 0)
            emit paramChanged(EquipmentParam_PrintError, error, slpa->errorDescription(error));
        else if(!isStateError(status))
            emit paramChanged(EquipmentParam_PrintError, 0, "");
        error = errorCode;
    }
}


