#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "tools.h"
#include "equipmentmanager.h"
#include "transactiondbtable.h"
#include "productdbtable.h"
#include "database.h"
#include "users.h"
#include "Slpa100u.h"
#include "Label/labelcreator.h"

EquipmentManager::EquipmentManager(QObject *parent, DataBase* dataBase, Settings* globalSettings):
    QObject(parent), db(dataBase), settings(globalSettings)
{
    Tools::debugLog("@@@@@ EquipmentManager::EquipmentManager ");
}

QString EquipmentManager::getAndClearMessage()
{
    QString s = message;
    message = "";
    return s;
}

bool EquipmentManager::isWM()
{
    return wm100 != nullptr && isWMStarted && WMMode != EquipmentMode_None;
}

bool EquipmentManager::isPM()
{
    return slpa != nullptr && isPMStarted && PMMode != EquipmentMode_None;
}

void EquipmentManager::create()
{
#define WM_MESSAGE_DEMO "\nДемо-режим весового модуля"
#define WM_MESSAGE_NONE "\nВесовой модуль не подключен"
#define PM_MESSAGE_DEMO "\nДемо-режим принтера"
#define PM_MESSAGE_NONE "\nПринтер не подключен"
#define WMPM_MESSAGE_NONE "\nОборудование не подключено"

    const QString path = EQUIPMENT_CONFIG_FILE;
    Tools::debugLog("@@@@@ EquipmentManager::create " + path);

    WMMode = PMMode = EquipmentMode_None;
    removeWM();
    removePM();

    if(!Tools::checkPermission("android.permission.READ_EXTERNAL_STORAGE"))
    {
        message += "\nНет разрешения для чтения конфиг.файла ";
        message += WMPM_MESSAGE_NONE;
        return;
    }
    if(!Tools::isFileExists(path))
    {
        message += "\nКонфиг.файл " + path + " не найден";
        message += WMPM_MESSAGE_NONE;
        return;
    }
    if(Tools::getFileSize(path) == 0)
    {
        message += "\nКонфиг.файл " + path + " имеет размер 0";
        message += WMPM_MESSAGE_NONE;
        return;
    }

    createWM();
    if(wm100 == nullptr)
    {
        removeWM();
        message += WMPM_MESSAGE_NONE;
        return;
    }
    int e1 = wm100->readConnectParam(path, "WmUri", WMUri);
    if(e1 != 0) message += "\n" + wm100->errorDescription(e1);
    else
    {
        switch (wm100->checkUri(WMUri))
        {
        case Wm100Protocol::diDemo:
            WMMode = EquipmentMode_Demo;
            message += WM_MESSAGE_DEMO;
            break;
        case Wm100Protocol::diNone:
            message += WM_MESSAGE_NONE;
            break;
        default:
            WMMode = EquipmentMode_Ok;
            break;
        }
    }

    createPM();
    int e2 = wm100->readConnectParam(path, "PrinterUri", PMUri);
    if(e2 != 0) message += "\n" + wm100->errorDescription(e2);
    else
    {
        switch (slpa->checkUri(PMUri))
        {
        case Slpa100uProtocol::diDemo:
            PMMode = EquipmentMode_Demo;
            message += PM_MESSAGE_DEMO;
            break;
        case Slpa100uProtocol::diNone:
            message += PM_MESSAGE_NONE;
            break;
        default:
            PMMode = EquipmentMode_Ok;
            break;
        }
    }
    if(WMMode == EquipmentMode_None) removeWM();
    if(PMMode == EquipmentMode_None) removePM();
}

void EquipmentManager::createWM()
{
    Tools::debugLog("@@@@@ EquipmentManager::createWM ");
    if(wm100 == nullptr)
    {
        wm100 = new Wm100(this);
        connect(wm100, &Wm100::weightStatusChanged, this, &EquipmentManager::onWMStatusChanged);
        connect(wm100, &Wm100::errorStatusChanged, this, &EquipmentManager::onWMErrorStatusChanged);
    }
 }

int EquipmentManager::startWM() // return error
{
    Tools::debugLog("@@@@@ EquipmentManager::startWM " + WMUri);
    createWM();
    int e = 0;
    if (wm100 != nullptr && !isWMStarted)
    {
        e = wm100->connectDevice(WMUri);
        isWMStarted = (e == 0);
        wm100->blockSignals(!isWMStarted);
        if(isWMStarted)
        {
            if(isSystemDateTime)
            {
                Tools::debugLog("@@@@@ EquipmentManager::startWM setSystemDateTime");
                wm100->setDateTime(QDateTime::currentDateTime());
            }
            wm100->startPolling(200);
        }
        isSystemDateTime = false;
    }
    if(e) message += QString("\nОшибка весового модуля %1: %2").arg(
                Tools::intToString(e), getWMErrorDescription(e));
    Tools::debugLog("@@@@@ EquipmentManager::startWM error " + Tools::intToString(e));
    return e;
}

void EquipmentManager::removeWM()
{
    Tools::debugLog("@@@@@ EquipmentManager::removeWM");
    if (wm100 != nullptr)
    {
        if(isWMStarted)
        {
            isWMStarted = false;
            wm100->blockSignals(true);
            wm100->stopPolling();
            int e = wm100->disconnectDevice();
            Tools::debugLog("@@@@@ EquipmentManager::removeWM error " + Tools::intToString(e));
        }
        disconnect(wm100, &Wm100::weightStatusChanged, this, &EquipmentManager::onWMStatusChanged);
        disconnect(wm100, &Wm100::errorStatusChanged, this, &EquipmentManager::onWMErrorStatusChanged);
        delete wm100;
        wm100 = nullptr;
    }
    isSystemDateTime = false;
}

QString EquipmentManager::WMversion() const
{
    if (wm100 != nullptr)
    {
        Wm100Protocol::device_metrics dm;
        if(wm100->getDeviceMetrics(&dm) >= 0) return Tools::intToString(dm.protocol_version);
    }
    return "-";
}

void EquipmentManager::setWMParam(const int param)
{
    Tools::debugLog("@@@@@ EquipmentManager::setWMParam " + Tools::intToString(param));
    if (wm100 == nullptr || !isWMStarted) return;
    switch (param)
    {
    case ControlParam_Tare:
        wm100->setTare();
        break;
    case ControlParam_Zero:
        wm100->setZero();
        wm100->setTare();
        break;
    default:
        break;
    }
}

bool EquipmentManager::isWMFlag(Wm100Protocol::channel_status s, int shift) const
{
    return (s.state & (0x00000001 << shift)) != 0;
}

bool EquipmentManager::isWMStateError(Wm100Protocol::channel_status s) const
{
    return isWMFlag(s, 5) || isWMFlag(s, 6) || isWMFlag(s, 7) || isWMFlag(s, 8) || isWMFlag(s, 9);
}

QString EquipmentManager::getWMErrorDescription(const int e) const
{
    switch(e)
    {
    case 0:    return "Ошибок нет";
    case 5003: return "Ошибка автонуля при включении";
    case 5004: return "Перегрузка по весу";
    case 5005: return "Ошибка при получении измерения";
    case 5006: return "Весы недогружены";
    case 5007: return "Ошибка: нет ответа от АЦП";
    //default: return "Неизвестная ошибка";
    }
    return wm100 == nullptr ? "Весовой модуль не подключен" : wm100->errorDescription(e);
}

void EquipmentManager::onWMStatusChanged(Wm100Protocol::channel_status &s)
{
    if(DEBUG_WEIGHT_STATUS)
        Tools::debugLog(QString("@@@@@EquipmentManager::onWMStatusChanged state=%1b weight=%2 tare=%3").arg(
            QString::number(s.state, 2), QString::number(s.weight), QString::number(s.tare)));

    //bool b0 = isFlag(s, 0); // признак фиксации веса
    //bool b1 = isFlag(s, 1); // признак работы автонуляmain()
    //bool b2 = isFlag(s, 2); // "0"- канал выключен, "1"- канал включен
    //bool b3 = isFlag(s, 3); // признак тары
    //bool b4 = isFlag(s, 4); // признак успокоения веса
    bool b5 = isWMFlag(s, 5); // ошибка автонуля при включении
    bool b6 = isWMFlag(s, 6); // перегрузка по весу
    bool b7 = isWMFlag(s, 7); // ошибка при получении измерения (нет градуировки весов или она не правильная)
    bool b8 = isWMFlag(s, 8); // весы недогружены
    bool b9 = isWMFlag(s, 9); // ошибка: нет ответа от АЦП

    ControlParam param = ControlParam_WeightValue;
    int e = 0;
    if(b5 || b6 || b7 || b8 || b9) // Ошибка состояния
    {
        param = ControlParam_WeightError;
        if(b5 && isWMFlag(WMStatus, 5) != b5) e = 5003;
        if(b6 && isWMFlag(WMStatus, 6) != b6) e = 5004;
        if(b7 && isWMFlag(WMStatus, 7) != b7) e = 5005;
        if(b8 && isWMFlag(WMStatus, 8) != b8) e = 5006;
        if(b9 && isWMFlag(WMStatus, 9) != b9) e = 5007;
    }
    else if(isWMStateError(WMStatus) && WMErrorCode == 0) param = ControlParam_WeightError; // Ошибка исчезла

    WMStatus.weight = s.weight;
    WMStatus.tare = s.tare;
    WMStatus.state = s.state;
    emit paramChanged(param, e);
}

void EquipmentManager::onWMErrorStatusChanged(int e)
{
    Tools::debugLog("@@@@@ EquipmentManager::onWMErrorStatusChanged " + QString::number(e));
    if(WMErrorCode != e)
    {
        WMErrorCode = e;
        emit paramChanged(ControlParam_WeightError, e);
    }
}

void EquipmentManager::createPM()
{
    if(slpa == nullptr)
    {
        Tools::debugLog("@@@@@ EquipmentManager::createPM ");
        slpa = new Slpa100u(this);
        labelCreator = new LabelCreator(this);
        connect(slpa, &Slpa100u::printerErrorChanged, this, &EquipmentManager::onPMErrorStatusChanged);
        connect(slpa, &Slpa100u::printerStatusChanged, this, &EquipmentManager::onPMStatusChanged);
    }
}

int EquipmentManager::startPM()
{
    Tools::debugLog("@@@@@ EquipmentManager::startPM " + PMUri);
    createPM();
    int e = 0;
    if (slpa != nullptr && labelCreator != nullptr && !isPMStarted)
    {
        e = slpa->connectDevice(PMUri);
        isPMStarted = (e == 0);
        slpa->blockSignals(!isPMStarted);
        if(isPMStarted)
        {
            slpa->startPolling(200);
            e = slpa->setBrightness(settings->getIntValue(SettingCode_PrinterBrightness) + 8);
            if(e >= 0) e = slpa->setOffset(settings->getIntValue(SettingCode_PrintOffset) + 8);
            if(e >= 0) e = slpa->setPaper(settings->getIntValue(SettingCode_PrintPaper) == 0 ?
                                          Slpa100uProtocol::papertype::ptSticker :
                                          Slpa100uProtocol::papertype::ptRibbon);
            if(e >= 0) e = slpa->setSensor(settings->getBoolValue(SettingCode_PrintLabelSensor));
        }
    }
    if(e) message += QString("\nОшибка принтера %1: %2").arg(
                Tools::intToString(e), getPMErrorDescription(e));
    Tools::debugLog("@@@@@ EquipmentManager::startPM error " + QString::number(e));
    return e;
}

void EquipmentManager::removePM()
{
    Tools::debugLog("@@@@@ EquipmentManager::removePM");
    if (slpa != nullptr)
    {
        if(isPMStarted)
        {
            slpa->blockSignals(true);
            slpa->stopPolling();
            int e = slpa->disconnectDevice();
            Tools::debugLog("@@@@@ EquipmentManager::removePM error " + QString::number(e));
            isPMStarted = false;
        }
        disconnect(slpa, &Slpa100u::printerErrorChanged, this, &EquipmentManager::onPMErrorStatusChanged);
        disconnect(slpa, &Slpa100u::printerStatusChanged, this, &EquipmentManager::onPMStatusChanged);
        delete labelCreator;
        labelCreator = nullptr;
        delete slpa;
        slpa = nullptr;
    }
}

QString EquipmentManager::PMversion() const
{
    return (slpa == nullptr) ? "-" : QString::number(slpa->getPrinterVersion());
}

void EquipmentManager::feed()
{
    Tools::debugLog("@@@@@ EquipmentManager::feed");
    if(!isPMStarted || slpa == nullptr) return;
    slpa->feed();
}

bool EquipmentManager::isPMStateError(uint16_t s) const
{
    if(slpa == nullptr) return false;
    bool b0 = isPMFlag(s, 0);
    bool b1 = isPMFlag(s, 1);
    bool b2 = isPMFlag(s, 2);
    bool b3 = isPMFlag(s, 3);
    bool b6 = isPMFlag(s, 6);
    bool b8 = isPMFlag(s, 8);
    return !b0 || !b2 || b3 || b8 || (b1 && b6);
}

QString EquipmentManager::getPMErrorDescription(const int e) const
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

void EquipmentManager::onPMStatusChanged(uint16_t s)
{
    Tools::debugLog(QString("@@@@@ EquipmentManager::onPMStatusChanged %1b").arg(QString::number(s, 2)));
    if(slpa == nullptr)
    {
        PMStatus = 0;
        return;
    }

    bool b0 = isPMFlag(s, 0);
    bool b1 = isPMFlag(s, 1);
    bool b2 = isPMFlag(s, 2);
    bool b3 = isPMFlag(s, 3);
    bool b6 = isPMFlag(s, 6);
    bool b8 = isPMFlag(s, 8);

    ControlParam param = ControlParam_None;
    int e = 0;
    if(!b0 || b1 || !b2 || b3 || b6 || b8)
    {
        param = ControlParam_PrintError;
        if(!b0 && isPMFlag(PMStatus, 0) != b0) e = 1003;
        if(!b2 && isPMFlag(PMStatus, 2) != b2) e = 1006;
        if(b3 && isPMFlag(PMStatus, 3) != b3) e = 1004;
        if(b8 && isPMFlag(PMStatus, 8) != b8) e = 1008;
        if(b1 && b6 && isPMFlag(PMStatus, 1) != b1) e = 1005;
    }
    else if(isPMStateError(PMStatus) && PMErrorCode == 0) param = ControlParam_PrintError; // Ошибка исчезла

    PMStatus = s;
    if(param != ControlParam_None) emit paramChanged(param, e);
}

void EquipmentManager::onPMErrorStatusChanged(int e)
{
    Tools::debugLog(QString("@@@@@ EquipmentManager::onPMErrorStatusChanged %1").arg(QString::number(e)));
    if(slpa != nullptr && PMErrorCode != e)
    {
        PMErrorCode = e;
        emit paramChanged(ControlParam_PrintError, e);
    }
}

QString EquipmentManager::parseBarcode(const QString& barcodeTemplate, const QChar c, const QString& value)
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
    Tools::debugLog(QString("@@@@@ EquipmentManager::parseBarcode %1 %2 %3 %4").arg(barcodeTemplate, value, result, Tools::intToString(n)));
    return result;
}

QString EquipmentManager::makeBarcode(const DBRecord& product, const QString& quantity, const QString& price, const QString& amount)
{
    QString result;
    QString barcodeTemplate = ProductDBTable::isPiece(product) ?
                settings->getStringValue(SettingCode_PrintLabelBarcodePiece) :
                settings->getStringValue(SettingCode_PrintLabelBarcodeWeight);
    if(barcodeTemplate.contains("P"))
    {
        result = ProductDBTable::isPiece(product) ?
                 settings->getStringValue(SettingCode_PrintLabelPrefixPiece) :
                 settings->getStringValue(SettingCode_PrintLabelPrefixWeight);
    }
    result += parseBarcode(barcodeTemplate, 'C', product[ProductDBTable::Code].toString()) +
              parseBarcode(barcodeTemplate, 'T', amount) +
              parseBarcode(barcodeTemplate, 'B', product[ProductDBTable::Barcode].toString()) +
              parseBarcode(barcodeTemplate, 'W', quantity);
    Tools::debugLog(QString("@@@@@ EquipmentManager::makeBarcode %1 %2").arg(barcodeTemplate, result));
    return result.length() != barcodeTemplate.length() ? "" : result;
}

int EquipmentManager::print(const DBRecord& user, const DBRecord& product,
                         const QString& quantity, const QString& price, const QString& amount)
{
    Tools::debugLog("@@@@@ EquipmentManager::print");
    if(!isPMStarted || slpa == nullptr) return 1007;

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
        pd.shop = settings->getStringValue(SettingCode_ShopName);
        pd.operatorcode = user[UserField_Code].toString();
        pd.operatorname = user[UserField_Name].toString();
        pd.date = Tools::dateFromUInt(dateTime, "dd.MM.yyyy");
        pd.time = Tools::timeFromUInt(dateTime, "hh:mm:ss");
        pd.labelnumber = QString::number(labelNumber);
        pd.scalesnumber = settings->getStringValue(SettingCode_ScalesNumber),
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
                        user[UserField_Code].toInt(),
                        Tools::stringToInt(product[ProductDBTable::Code]),
                        labelNumber,
                        Tools::stringToDouble(quantity),
                        Tools::stringToInt(price),
                        Tools::stringToInt(amount));
            emit printed(r);
        }
        else Tools::debugLog("@@@@@ EquipmentManager::print ERROR (get Transactions Table)");
    }
    if(e != 0) message += "\nОшибка печати " + getPMErrorDescription(e);
    else if(isPMDemoMode()) message += "\nДемо-печать";
    return e;
}
