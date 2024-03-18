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
    Tools::debugLog("@@@@@ PrintManager::PrintManager " + Tools::boolToString(demo));
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
    int e = 0;
    if (slpa != nullptr && labelCreator != nullptr && !started)
    {
        Tools::debugLog("@@@@@ PrintManager::start " + url);
        e = slpa->connectDevice(url);
        Tools::debugLog("@@@@@ PrintManager::start connect device error " + QString::number(e));
        started = (e == 0);
        if(started) slpa->startPolling(200);
        Tools::debugLog("@@@@@ PrintManager::start Done");
    }
    return e;
}

void PrintManager::stop()
{
    if (slpa != nullptr && started)
    {
        Tools::debugLog("@@@@@ PrintManager::stop");
        slpa->stopPolling();
        int e = slpa->disconnectDevice();
        Tools::debugLog("@@@@@ PrintManager::stop disconnect device error " + QString::number(e));
        started = false;
        Tools::debugLog("@@@@@ PrintManager::stop Done");
    }
}

QString PrintManager::version() const
{
    return (slpa == nullptr) ? "DEMO" : QString::number(slpa->getPrinterVersion());
}

void PrintManager::feed()
{
    Tools::debugLog("@@@@@ PrintManager::feed");
    if(!started || slpa == nullptr) return;
    slpa->feed();
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

int PrintManager::setParams(const int brightness, const int offset) // -7 .. +7
{
    Tools::debugLog(QString("@@@@@ PrintManager::setParams %1 %2").arg(QString::number(brightness), QString::number(offset)));
    if(!started || slpa == nullptr) return 0;
    int e = slpa->setBrightness(brightness + 8);
    if(e >= 0) e = slpa->setOffset(offset + 8);
    return e;
}

void PrintManager::onStatusChanged(uint16_t s)
{
    Tools::debugLog(QString("@@@@@ PrintManager::onStatusChanged %1").arg(s));
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
    Tools::debugLog("@@@@@ PrintManager::onErrorStatusChanged " + QString::number(e));
    if(slpa != nullptr && errorCode != e)
    {
        errorCode = e;
        emit paramChanged(EquipmentParam_PrintError, e);
    }
}

void PrintManager::print(const DBRecord& user, const DBRecord& product,
                         const QString& quantity, const QString& price, const QString& amount)
{
    Tools::debugLog("@@@@@ PrintManager::print");
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
        pd.shop = settings.getItemStringValue(SettingCode_ShopName);
        pd.operatorcode = user[UserDBTable::Code].toString();
        pd.operatorname = user[UserDBTable::Name].toString();
        pd.date = Tools::dateFromUInt(dateTime, "dd.MM.yyyy");
        pd.time = Tools::timeFromUInt(dateTime, "hh:mm:ss");
        pd.labelnumber = QString::number(labelNumber);
        pd.scalesnumber = settings.getItemStringValue(SettingCode_ScalesNumber),
        pd.picturefile = ""; // todo
        pd.textfile = ""; // todo

        QImage p = labelCreator->createImage(pd);
        e = slpa->print(p);
    }
    if(e == 0)
    {
        TransactionDBTable* t = (TransactionDBTable*)db->getTable(DBTABLENAME_TRANSACTIONS);
        if(t != nullptr)
        {
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
        else Tools::debugLog("@@@@@ PrintManager::print ERROR (get Transactions Table)");
    }
}



