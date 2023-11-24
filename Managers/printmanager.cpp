#include <QDebug>
#include "printmanager.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productdbtable.h"
#include "tools.h"
#include "database.h"
#include "Slpa100u.h"
#include "Label/labelcreator.h"

PrintManager::PrintManager(QObject *parent, DataBase* dataBase, Settings& globalSettings, const bool demo):
    QObject(parent), db(dataBase), settings(globalSettings)
{
    qDebug() << "@@@@@ PrintManager::PrintManager " << demo;
    if(!demo)
    {
        slpa = new Slpa100u(this);
        labelCreator = new LabelCreator(this);
        connect(slpa, &Slpa100u::printerErrorChanged, this, &PrintManager::onErrorStatusChanged);
        connect(slpa, &Slpa100u::printerStatusChanged, this, &PrintManager::onStatusChanged);
    }
}

int PrintManager::start(const QString& url)
{
    qDebug() << "@@@@@ PrintManager::start ";
    int e = 0;
    if (slpa != nullptr && labelCreator != nullptr && !started)
    {
        e = slpa->connectDevice(url);
        started = (e == 0);
        if(started) slpa->startPolling(200);
        qDebug() << "@@@@@ PrintManager::start error = " << e;
    }
    return e;
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

QString PrintManager::version() const
{
    return (slpa != nullptr) ? QString::number(slpa->getPrinterVersion()) : "?";
}

void PrintManager::feed()
{
    qDebug() << "@@@@@ PrintManager::feed";
    if(!started || slpa == nullptr) return;
    slpa->feed();
    // int e = slpa->feed(); todo
}

bool PrintManager::isStateError(uint16_t s) const
{
    if(slpa == nullptr) return false;
    bool b0 = isFlag(s, 0);
    bool b1 = isFlag(s, 1);
    bool b2 = isFlag(s, 2);
    bool b3 = isFlag(s, 3);
    bool b6 = isFlag(s, 6);
    bool b8 = isFlag(s, 8);
    return !b0 || !b2 || b3 || b8 || (b1 && b6);
}

QString PrintManager::getErrorDescription(const int e) const
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
    if(slpa == nullptr)
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
    if(!b0 || b1 || !b2 || b3 || b6 || b8)
    {
        param = EquipmentParam_PrintError;
        if(!b0 && isFlag(status, 0) != b0) e = 1003;
        if(!b2 && isFlag(status, 2) != b2) e = 1006;
        if(b3 && isFlag(status, 3) != b3) e = 1004;
        if(b8 && isFlag(status, 8) != b8) e = 1008;
        if(b1 && b6 && isFlag(status, 1) != b1) e = 1005;
    }
    else if(isStateError(status) && errorCode == 0) param = EquipmentParam_PrintError; // Ошибка исчезла

    status = s;
    if(param != EquipmentParam_None) emit paramChanged(param, e);
}

void PrintManager::onErrorStatusChanged(int e)
{
    qDebug() << "@@@@@ PrintManager::onErrorStatusChanged " << e;
    if(slpa != nullptr && errorCode != e)
    {
        errorCode = e;
        emit paramChanged(EquipmentParam_PrintError, e);
    }
}

void PrintManager::print(const DBRecord& user, const DBRecord& product,
                         const QString& quantity, const QString& price, const QString& amount)
{
    qDebug() << "@@@@@ PrintManager::print";
    if(!started || slpa == nullptr) return;

    quint64 dateTime = Tools::currentDateTimeToUInt();
    int labelNumber = 0; // todo
    int e = labelCreator->loadLabel(":/Labels/60x40.lpr"); // todo
    //int e = slpa->printTest(100);
    if (e == 0)
    {
        PrintData pd;
        pd.weight = quantity;
        pd.price = price;
        pd.cost = amount;
        pd.tare = product[ProductDBTable::Tare].toString();
        pd.barcode = product[ProductDBTable::Barcode].toString();
        pd.itemcode = product[ProductDBTable::Code].toString();
        pd.name = product[ProductDBTable::Name].toString();
        pd.shelflife = product[ProductDBTable::Shelflife].toString();
        pd.validity = ""; // todo
        pd.price2 = product[ProductDBTable::Price2].toString();
        pd.certificate = product[ProductDBTable::Certificate].toString();
        pd.message = db->getProductMessageById(product[ProductDBTable::MessageCode].toString());
        pd.shop = settings.getItemStringValue(SettingDBTable::SettingCode_ShopName);
        pd.operatorcode = user[UserDBTable::Code].toString();
        pd.operatorname = user[UserDBTable::Name].toString();
        pd.date = Tools::dateFromUInt(dateTime);
        pd.time = Tools::timeFromUInt(dateTime);
        pd.labelnumber = QString::number(labelNumber);
        pd.scalesnumber = settings.getItemStringValue(SettingDBTable::SettingCode_ScalesNumber),
        pd.picturefile = ""; // todo
        pd.textfile = ""; // todo

        QImage p = labelCreator->createImage(pd);
        e = slpa->print(p);
    }
    if(e == 0)
    {
        TransactionDBTable* t = (TransactionDBTable*)db->getTableByName(DBTABLENAME_TRANSACTIONS);
        DBRecord r = t->createRecord(
                    dateTime,
                    user[UserDBTable::Code].toInt(),
                    Tools::stringToInt(product[ProductDBTable::Code]),
                    labelNumber,
                    Tools::stringToDouble(quantity),
                    Tools::stringToInt(price),
                    Tools::stringToInt(amount));
        emit printed(r);
    }
    /*
    else
    {
        QString s = QString("Ошибка печати %1").arg(error);
        emit showMessageBox("Внимание!", s, true);
    }
    */
}



