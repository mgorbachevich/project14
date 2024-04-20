#include <QDebug>
#include <QTimer>
#include "printmanager.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productdbtable.h"
#include "tools.h"
#include "database.h"
#include "Slpa100u.h"
#include "Label/labelcreator.h"

PrintManager::PrintManager(QObject *parent, DataBase* dataBase, Settings& globalSettings):
    QObject(parent), db(dataBase), settings(globalSettings)
{
    Tools::debugLog("@@@@@ PrintManager::PrintManager ");
}

int PrintManager::start(const QString& uri)
{
    Tools::debugLog("@@@@@ PrintManager::start " + uri);
    if((0 != QString::compare(uri, EQUIPMENT_OFF_URI)) && slpa == nullptr)
    {
        slpa = new Slpa100u(this);
        labelCreator = new LabelCreator(this);
        connect(slpa, &Slpa100u::printerErrorChanged, this, &PrintManager::onErrorStatusChanged);
        connect(slpa, &Slpa100u::printerStatusChanged, this, &PrintManager::onStatusChanged);
    }
    demo = (0 == QString::compare(uri, PRINTER_DEMO_URI));
    int e = 0;
    if (slpa != nullptr && labelCreator != nullptr && !started)
    {
        e = slpa->connectDevice(uri);
        started = (e == 0);
        slpa->blockSignals(!started);
        if(started)
        {
            slpa->startPolling(200);
            e = slpa->setBrightness(settings.getIntValue(SettingCode_PrinterBrightness) + 8);
            if(e >= 0) e = slpa->setOffset(settings.getIntValue(SettingCode_PrintOffset) + 8);
            if(e >= 0) e = slpa->setPaper(settings.getIntValue(SettingCode_PrintPaper) == 0 ?
                                          Slpa100uProtocol::papertype::ptSticker :
                                          Slpa100uProtocol::papertype::ptRibbon);
            if(e >= 0) e = slpa->setSensor(settings.getBoolValue(SettingCode_PrintLabelSensor));
        }
    }
    Tools::debugLog("@@@@@ PrintManager::start error " + QString::number(e));
    return e;
}

void PrintManager::stop()
{
    Tools::debugLog("@@@@@ PrintManager::stop");
    if (slpa != nullptr)
    {
        if(started)
        {
            slpa->blockSignals(true);
            slpa->stopPolling();
            int e = slpa->disconnectDevice();
            Tools::debugLog("@@@@@ PrintManager::stop error " + QString::number(e));
            started = false;
        }
        disconnect(slpa, &Slpa100u::printerErrorChanged, this, &PrintManager::onErrorStatusChanged);
        disconnect(slpa, &Slpa100u::printerStatusChanged, this, &PrintManager::onStatusChanged);
        delete labelCreator;
        labelCreator = nullptr;
        delete slpa;
        slpa = nullptr;
    }
}

QString PrintManager::version() const
{
    return (slpa == nullptr) ? "-" : QString::number(slpa->getPrinterVersion());
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
    case 1007: return "Ошибка принтера!";
    case 1008: return "Ошибка памяти принтера!";
    case 1009: return "Неверный штрихкод";
    }
    return slpa == nullptr ? "" : slpa->errorDescription(e);
}

void PrintManager::onStatusChanged(uint16_t s)
{
    Tools::debugLog(QString("@@@@@ PrintManager::onStatusChanged %1b").arg(QString::number(s, 2)));
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

    ControlParam param = ControlParam_None;
    int e = 0;
    if(!b0 || b1 || !b2 || b3 || b6 || b8)
    {
        param = ControlParam_PrintError;
        if(!b0 && isFlag(status, 0) != b0) e = 1003;
        if(!b2 && isFlag(status, 2) != b2) e = 1006;
        if(b3 && isFlag(status, 3) != b3) e = 1004;
        if(b8 && isFlag(status, 8) != b8) e = 1008;
        if(b1 && b6 && isFlag(status, 1) != b1) e = 1005;
    }
    else if(isStateError(status) && errorCode == 0) param = ControlParam_PrintError; // Ошибка исчезла

    status = s;
    if(param != ControlParam_None) emit paramChanged(param, e);
}

void PrintManager::onErrorStatusChanged(int e)
{
    Tools::debugLog(QString("@@@@@ PrintManager::onErrorStatusChanged %1").arg(QString::number(e)));
    if(slpa != nullptr && errorCode != e)
    {
        errorCode = e;
        emit paramChanged(ControlParam_PrintError, e);
    }
}

QString PrintManager::parseBarcode(const QString& barcodeTemplate, const QChar c, const QString& value)
{
    QString result;
    int n = 0;
    for(int i = 0; i < barcodeTemplate.length(); i++) if(barcodeTemplate.at(i) == c) n++;
    if(n > 0)
    {
        QString v = value;
        v = v.replace(QString("."), QString("")).replace(QString(","), QString(""));
        if(n >= v.length())
        {
            for(int i = 0; i < n - v.length(); i++) result += "0";
            result += v;
        }
    }
    Tools::debugLog(QString("@@@@@ PrintManager::parseBarcode %1 %2 %3 %4").arg(barcodeTemplate, value, result, Tools::intToString(n)));
    return result;
}

QString PrintManager::makeBarcode(const DBRecord& product, const QString& quantity, const QString& price, const QString& amount)
{
    QString result;
    QString barcodeTemplate = ProductDBTable::isPiece(product) ?
                settings.getStringValue(SettingCode_PrintLabelBarcodePiece) :
                settings.getStringValue(SettingCode_PrintLabelBarcodeWeight);
    if(barcodeTemplate.contains("P"))
    {
        result = ProductDBTable::isPiece(product) ?
                 settings.getStringValue(SettingCode_PrintLabelPrefixPiece) :
                 settings.getStringValue(SettingCode_PrintLabelPrefixWeight);
    }
    result += parseBarcode(barcodeTemplate, 'C', price) +
              parseBarcode(barcodeTemplate, 'T', amount) +
              parseBarcode(barcodeTemplate, 'B', product[ProductDBTable::Barcode].toString()) +
              parseBarcode(barcodeTemplate, 'W', quantity);
    Tools::debugLog(QString("@@@@@ PrintManager::makeBarcode %1 %2").arg(barcodeTemplate, result));
    return result.length() != barcodeTemplate.length() ? "" : result;
}

int PrintManager::print(const DBRecord& user, const DBRecord& product,
                         const QString& quantity, const QString& price, const QString& amount)
{
    Tools::debugLog("@@@@@ PrintManager::print");
    if(!started || slpa == nullptr) return 1007;

    QString barcode = makeBarcode(product, quantity, price, amount);
    if (barcode.isEmpty()) return 1009;

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
        pd.barcode = barcode;
        pd.itemcode = product[ProductDBTable::Code].toString();
        pd.name = product[ProductDBTable::Name].toString();
        pd.shelflife = product[ProductDBTable::Shelflife].toString();
        pd.validity = ""; // todo
        pd.price2 = product[ProductDBTable::Price2].toString();
        pd.certificate = product[ProductDBTable::Certificate].toString();
        pd.message = db->getProductMessageById(product[ProductDBTable::MessageCode].toString());
        pd.shop = settings.getStringValue(SettingCode_ShopName);
        pd.operatorcode = user[UserDBTable::Code].toString();
        pd.operatorname = user[UserDBTable::Name].toString();
        pd.date = Tools::dateFromUInt(dateTime, "dd.MM.yyyy");
        pd.time = Tools::timeFromUInt(dateTime, "hh:mm:ss");
        pd.labelnumber = QString::number(labelNumber);
        pd.scalesnumber = settings.getStringValue(SettingCode_ScalesNumber),
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
    return e;
}



