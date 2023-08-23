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
    if(demoMode) errorCode = -1;
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
        emit paramChanged(EquipmentParam_PrintError, 0, NO_ERROR_DESCRIPTION_TEXT);
        return;
    }

    bool b0 = isFlag(s, 0);
    bool b1 = isFlag(s, 1);
    bool b2 = isFlag(s, 2);
    bool b3 = isFlag(s, 3);
    bool b6 = isFlag(s, 6);
    bool b8 = isFlag(s, 8);

    EquipmentParam param = EquipmentParam_None;
    int e = 0;
    QString description = NO_ERROR_DESCRIPTION_TEXT;
    if(b0 || b1 || b2 || b3 || b6 || b8)
    {
        param = EquipmentParam_PrintError;
        if(b0 && isFlag(status, 0) != b0)
        {
            e = 1003;
            description = "Нет бумаги! Установите новый рулон!";
        }
        if(b1 && b6 && isFlag(status, 1) != b1)
        {
            e = 1005;
            description = "Снимите этикетку!";
        }
        if(b2 && isFlag(status, 2) != b2)
        {
            e = 1006;
            description = "Этикетка не спозиционирована! Нажмите клавишу промотки!";
        }
        if(b3 && isFlag(status, 3) != b3)
        {
            e = 1004;
            description = "Закройте головку принтера!";
        }
        if(b8 && isFlag(status, 8) != b8)
        {
            e = 1008;
            description = "Ошибка памяти принтера!";
        }
    }
    else
        if(isStateError(status) && errorCode == 0)
            param = EquipmentParam_PrintError; // Ошибка исчезла

    status = s;
    emit paramChanged(param, e, description);
}

void PrintManager::onErrorStatusChanged(int code)
{
    if(demoMode) return;

    if(errorCode != code)
    {
        errorCode = code;
        QString description = code == 0? NO_ERROR_DESCRIPTION_TEXT : slpa->errorDescription(code);
        emit paramChanged(EquipmentParam_PrintError, code, description);
    }
}


