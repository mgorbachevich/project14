#include <QDebug>
#include "printmanager.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productdbtable.h"
#include "tools.h"
#include "database.h"

PrintManager::PrintManager(QObject *parent, const bool demo): QObject(parent)
{
    qDebug() << "@@@@@ PrintManager::PrintManager " << demo;
    demoMode = demo;
    slpa = new Slpa100u(this);
    connect(slpa, &Slpa100u::printerErrorChanged, this, &PrintManager::onErrorStatusChanged);
    connect(slpa, &Slpa100u::printerStatusChanged, this, &PrintManager::onStatusChanged);
}

int PrintManager::start(const QString& url)
{
    qDebug() << "@@@@@ PrintManager::start ";
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
    qDebug() << "@@@@@ PrintManager::print";
    if(started && manualPrintEnabled)
    {
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
        productPrinted = true;
    }
}

void PrintManager::feed()
{
    qDebug() << "@@@@@ PrintManager::feed";
    if(!started || demoMode) return;
    slpa->feed();
    // int e = slpa->feed(); todo
}

bool PrintManager::isStateError(uint16_t s)
{
    if(demoMode) return false;
    return isFlag(s, 0) || (isFlag(s, 1) && isFlag(s, 6)) || isFlag(s, 2) || isFlag(s, 3) || isFlag(s, 8);
}

QString PrintManager::getErrorDescription(const int e)
{
    switch(e)
    {
    case 0: return "Ошибок нет";
    case 1003: return "Нет бумаги! Установите новый рулон!";
    case 1004: return "Закройте головку принтера!";
    case 1005: return "Снимите этикетку!";
    case 1006: return "Этикетка не спозиционирована! Нажмите клавишу промотки!";
    case 1008: return "Ошибка памяти принтера!";
    }
    return slpa == nullptr ? "" : slpa->errorDescription(e);
}

void PrintManager::onStatusChanged(uint16_t s)
{
    qDebug() << QString("@@@@@ PrintManager::onStatusChanged %1b").arg(s);
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

    EquipmentParam param = EquipmentParam_None;
    int e = 0;
    if(b0 || b1 || b2 || b3 || b6 || b8)
    {
        param = EquipmentParam_PrintError;
        if(b0 && isFlag(status, 0) != b0) e = 1003;
        if(b1 && b6 && isFlag(status, 1) != b1) e = 1005;
        if(b2 && isFlag(status, 2) != b2) e = 1006;
        if(b3 && isFlag(status, 3) != b3) e = 1004;
        if(b8 && isFlag(status, 8) != b8) e = 1008;
    }
    else if(isStateError(status) && errorCode == 0) param = EquipmentParam_PrintError; // Ошибка исчезла

    status = s;
    if(param != EquipmentParam_None) emit paramChanged(param, e);
}

void PrintManager::onErrorStatusChanged(int e)
{
    qDebug() << "@@@@@ PrintManager::onErrorStatusChanged " << e;
    if(!demoMode && errorCode != e)
    {
        errorCode = e;
        emit paramChanged(EquipmentParam_PrintError, e);
    }
}


