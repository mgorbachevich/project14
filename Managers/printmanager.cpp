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
        QString s = QString("Ошибка печати %1").arg(error);
        //emit showMessageBox("Внимание!", s, true);
    }
}

void PrintManager::onStatusChanged(uint16_t)
{

}

void PrintManager::onErrorStatusChanged(int errorCode)
{
    qDebug() << "@@@@@ PrintManager::onErrorStatusChanged ";
    error = errorCode;
    emit paramChanged(PrintParam_Error, QString::number(error), slpa->errorDescription(error));
}


