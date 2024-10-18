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
#include "appmanager.h"

EquipmentManager::EquipmentManager(AppManager *parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ EquipmentManager::EquipmentManager ");
}

EquipmentManager::~EquipmentManager()
{
    Tools::debugLog("@@@@@ EquipmentManager::~EquipmentManager ");
    removeWM();
    removePM();
}

bool EquipmentManager::isWM()
{
    return wm != nullptr && isWMStarted;
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
        showAttention(QString("Нет разрешения для чтения конфиг.файла. %1").arg(WMPM_MESSAGE_NONE));
        return;
    }
    if(!Tools::isFileExists(path))
    {
        showAttention(QString("Конфиг.файл %1 не найден. %2").arg(path, WMPM_MESSAGE_NONE));
        return;
    }
    if(Tools::getFileSize(path) == 0)
    {
        showAttention(QString("Конфиг.файл %1 имеет размер 0. %2").arg(path, WMPM_MESSAGE_NONE));
        return;
    }

    createWM();
    if(wm == nullptr)
    {
        removeWM();
        showAttention(WMPM_MESSAGE_NONE);
    }
    else
    {
        int e1 = wm->readConnectParam(path, "WmUri", WMUri);
        if(e1 != 0) showAttention(wm->errorDescription(e1));
        else
        {
            switch (wm->checkUri(WMUri))
            {
            case Wm100Protocol::diDemo:
                WMMode = EquipmentMode_Demo;
                showAttention(WM_MESSAGE_DEMO);
                break;
            case Wm100Protocol::diNone:
                showAttention(WMPM_MESSAGE_NONE);
                break;
            default:
                WMMode = EquipmentMode_Ok;
                break;
            }
        }
    }

    createPM();
    int e2 = wm->readConnectParam(path, "PrinterUri", PMUri);
    if(e2 != 0) showAttention(wm->errorDescription(e2));
    else
    {
        switch (slpa->checkUri(PMUri))
        {
        case Slpa100uProtocol::diDemo:
            PMMode = EquipmentMode_Demo;
            showAttention(PM_MESSAGE_DEMO);
            break;
        case Slpa100uProtocol::diNone:
            showAttention(PM_MESSAGE_NONE);
            break;
        default:
            PMMode = EquipmentMode_Ok;
            break;
        }
    }
    if(WMMode == EquipmentMode_None) removeWM();
    if(PMMode == EquipmentMode_None) removePM();
}

void EquipmentManager::start()
{
    Tools::debugLog("@@@@@ EquipmentManager::start ");
    startWM();
    startPM();
    pause(false);
}

void EquipmentManager::stop()
{
    Tools::debugLog("@@@@@ EquipmentManager::stop ");
    pause(true);
    stopWM();
    stopPM();
}

void EquipmentManager::createWM()
{
    if(wm == nullptr)
    {
        Tools::debugLog("@@@@@ EquipmentManager::createWM ");
        wm = new Wm100(this);
        connect(wm, &Wm100::weightStatusChanged, this, &EquipmentManager::onWMStatusChanged);
        connect(wm, &Wm100::errorStatusChanged, this, &EquipmentManager::onWMErrorStatusChanged);
    }
 }

int EquipmentManager::startWM() // return error
{
    Tools::debugLog("@@@@@ EquipmentManager::startWM " + WMUri);
    createWM();
    int e = 0;
    if (wm != nullptr && !isWMStarted)
    {
        e = wm->connectDevice(WMUri);
        isWMStarted = (e == 0);
    }
    if(e) showAttention(QString("\nОшибка весового модуля %1: %2").arg(
                Tools::toString(e), getWMErrorDescription(e)));
    Tools::debugLog("@@@@@ EquipmentManager::startWM error " + Tools::toString(e));
    return e;
}

void EquipmentManager::stopWM()
{
    if (isWM())
    {
        Tools::debugLog("@@@@@ EquipmentManager::stopWM");
        isWMStarted = false;
        wm->blockSignals(true);
        wm->stopPolling();
        int e = wm->disconnectDevice();
        Tools::debugLog("@@@@@ EquipmentManager::stopWM error " + Tools::toString(e));
    }
}

void EquipmentManager::removeWM()
{
    Tools::debugLog("@@@@@ EquipmentManager::removeWM");
    stopWM();
    if (wm != nullptr)
    {
        disconnect(wm, &Wm100::weightStatusChanged, this, &EquipmentManager::onWMStatusChanged);
        disconnect(wm, &Wm100::errorStatusChanged, this, &EquipmentManager::onWMErrorStatusChanged);
        delete wm;
        wm = nullptr;
    }
}

void EquipmentManager::pauseWM(const bool v)
{
    Tools::debugLog("@@@@@ EquipmentManager::pauseWM " + Tools::toIntString(v));
    if(isWM())
    {
        if(v) wm->stopPolling();
        else wm->startPolling(EQUIPMENT_POLLING_INTERVAL);
        wm->blockSignals(v);

    }
}

void EquipmentManager::createPM()
{
    if(slpa == nullptr)
    {
        Tools::debugLog("@@@@@ EquipmentManager::createPM ");
        slpa = new Slpa100u(this);
        labelCreator = new LabelCreator(this);
        labelPath = "";
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
        //slpa->blockSignals(!isPMStarted);
        if(isPMStarted)
        {
            //slpa->startPolling(200);
            e = slpa->setBrightness(appManager->settings->getIntValue(SettingCode_PrinterBrightness) + 8);
            if(e >= 0) e = slpa->setOffset(appManager->settings->getIntValue(SettingCode_PrintOffset) + 8);
            if(e >= 0) e = slpa->setPaper(appManager->settings->getIntValue(SettingCode_PrintPaper, true) == 0 ?
                                          Slpa100uProtocol::papertype::ptSticker :
                                          Slpa100uProtocol::papertype::ptRibbon);
            if(e >= 0) e = slpa->setSensor(appManager->settings->getBoolValue(SettingCode_PrintLabelSensor));
        }
    }
    if(e) showAttention(QString("\nОшибка принтера %1: %2").arg(
                Tools::toString(e), getPMErrorDescription(e)));
    Tools::debugLog("@@@@@ EquipmentManager::startPM error " + QString::number(e));
    return e;
}

void EquipmentManager::stopPM()
{
    if (slpa != nullptr && isPMStarted)
    {
        Tools::debugLog("@@@@@ EquipmentManager::stopPM");
        slpa->blockSignals(true);
        slpa->stopPolling();
        int e = slpa->disconnectDevice();
        Tools::debugLog("@@@@@ EquipmentManager::stopPM error " + QString::number(e));
        isPMStarted = false;
    }
}

void EquipmentManager::removePM()
{
    Tools::debugLog("@@@@@ EquipmentManager::removePM");
    stopPM();
    if (slpa != nullptr)
    {
        disconnect(slpa, &Slpa100u::printerErrorChanged, this, &EquipmentManager::onPMErrorStatusChanged);
        disconnect(slpa, &Slpa100u::printerStatusChanged, this, &EquipmentManager::onPMStatusChanged);
        delete labelCreator;
        labelCreator = nullptr;
        labelPath = "";
        delete slpa;
        slpa = nullptr;
    }
}

void EquipmentManager::pausePM(const bool v)
{
    Tools::debugLog("@@@@@ EquipmentManager::pausePM " + Tools::toIntString(v));
    if(slpa != nullptr && isPMStarted)
    {
        if(v) slpa->stopPolling();
        else slpa->startPolling(EQUIPMENT_POLLING_INTERVAL);
        slpa->blockSignals(v);
    }
}

void EquipmentManager::setSystemDateTime(const QDateTime& dt)
{
    if (wm != nullptr && dt.isValid())
    {
        Tools::debugLog("@@@@@ EquipmentManager::setSystemDateTime " + dt.toString("dd.MM.yyyy HH:mm:ss"));
        if(isWMStarted)
            wm->setDateTime(dt);
        else
        {
            startWM();
            wm->setDateTime(dt);
            stopWM();
        }
    }
}

QString EquipmentManager::daemonVersion() const
{
    if (wm != nullptr)
    {
        QString s1, s2;
        wm->getDaemonVersion(s1, s2);
        if(!s1.isEmpty() || !s2.isEmpty()) return QString("%1 от %2").arg(s1, s2);
    }
    return "Не определено";
}

QString EquipmentManager::WMVersion() const
{
    if (wm != nullptr)
    {
        Wm100Protocol::device_metrics dm;
        wm->getDeviceMetrics(&dm);
        return dm.name;
    }
    return "Не определено";
}

void EquipmentManager::setTare()
{
    if (isWM())
    {
        Tools::debugLog("@@@@@ EquipmentManager::setTare ");
        wm->setTare();
    }
}

void EquipmentManager::setTare(const double v)
{
    if (isWM())
    {
        Tools::debugLog("@@@@@ EquipmentManager::setTare " +
                        Tools::toString(v, appManager->calculator->weightPointPosition()));
        wm->setTareValue(v);
    }
}

void EquipmentManager::setZero()
{
   if (isWM())
    {
       Tools::debugLog("@@@@@ EquipmentManager::setZero ");
        wm->setZero();
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
    return wm == nullptr ? "Весовой модуль не подключен" : wm->errorDescription(e);
}

void EquipmentManager::onWMStatusChanged(Wm100Protocol::channel_status &s)
{
#ifdef DEBUG_WEIGHT_STATUS
        Tools::debugLog(QString("@@@@@EquipmentManager::onWMStatusChanged state=%1b weight=%2 tare=%3").arg(
            QString::number(s.state, 2), QString::number(s.weight), QString::number(s.tare)));
#endif
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

    EquipmentParam param = EquipmentParam_WeightValue;
    int e = 0;
    if(b5 || b6 || b7 || b8 || b9) // Ошибка состояния
    {
        param = EquipmentParam_WeightError;
        if(b5 && isWMFlag(WMStatus, 5) != b5) e = Error_WM_AutoZero;
        if(b6 && isWMFlag(WMStatus, 6) != b6) e = Error_WM_Overload;
        if(b7 && isWMFlag(WMStatus, 7) != b7) e = Error_WM_Mesure;
        if(b8 && isWMFlag(WMStatus, 8) != b8) e = Error_WM_Underload;
        if(b9 && isWMFlag(WMStatus, 9) != b9) e = Error_WM_NoReply;
    }
    else if(isWMStateError(WMStatus) && WMErrorCode == 0) param = EquipmentParam_WeightError; // Ошибка исчезла

    WMStatus.weight = s.weight;
    WMStatus.tare = s.tare;
    WMStatus.state = s.state;
    emit paramChanged(param, e, getWMErrorDescription(e));
}

void EquipmentManager::onWMErrorStatusChanged(int e)
{
    Tools::debugLog("@@@@@ EquipmentManager::onWMErrorStatusChanged " + QString::number(e));
    if(WMErrorCode != e)
    {
        WMErrorCode = e;
        emit paramChanged(EquipmentParam_WeightError, e, getWMErrorDescription(e));
    }
}

QString EquipmentManager::PMVersion() const
{
    if(slpa != nullptr)
    {
        int v = slpa->getPrinterVersion();
        QByteArray ba = Tools::toBytes((quint32)v);
        return QString("%1.%2").arg(Tools::toString((int)ba[2]), Tools::toString((int)ba[3]));
    }
    return "Не определено";
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
    case Error_PM_NoPaper: return "Нет бумаги! Установите новый рулон!";
    case Error_PM_Opened: return "Закройте головку принтера!";
    case Error_PM_GetLabel: return "Снимите этикетку!";
    case Error_PM_BadPosition: return "Этикетка не спозиционирована! Нажмите клавишу промотки!";
    case Error_PM_Fail: return "Ошибка принтера!";
    case Error_PM_Memory: return "Ошибка памяти принтера!";
    case Error_PM_Barcode: return "Неверный штрихкод";
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

    EquipmentParam param = EquipmentParam_None;
    int e = 0;
    if(!b0 || b1 || !b2 || b3 || b6 || b8)
    {
        param = EquipmentParam_PrintError;
        if(!b0 && isPMFlag(PMStatus, 0) != b0)      e = Error_PM_NoPaper;
        if(!b2 && isPMFlag(PMStatus, 2) != b2)      e = Error_PM_BadPosition;
        if(b3 && isPMFlag(PMStatus, 3) != b3)       e = Error_PM_Opened;
        if(b8 && isPMFlag(PMStatus, 8) != b8)       e = Error_PM_Memory;
        if(b1 && b6 && isPMFlag(PMStatus, 1) != b1) e = Error_PM_GetLabel;
    }
    else if(isPMStateError(PMStatus) && PMErrorCode == 0) param = EquipmentParam_PrintError; // Ошибка исчезла

    PMStatus = s;
    if(param != EquipmentParam_None) emit paramChanged(param, e, getPMErrorDescription(e));
}

void EquipmentManager::onPMErrorStatusChanged(int e)
{
    Tools::debugLog(QString("@@@@@ EquipmentManager::onPMErrorStatusChanged %1").arg(QString::number(e)));
    if(slpa != nullptr && PMErrorCode != e)
    {
        PMErrorCode = e;
        emit paramChanged(EquipmentParam_PrintError, e, getPMErrorDescription(e));
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
    Tools::debugLog(QString("@@@@@ EquipmentManager::parseBarcode %1 %2 %3 %4").arg(barcodeTemplate, value, result, Tools::toString(n)));
    return result;
}

QString EquipmentManager::makeBarcode(const DBRecord& product, const QString& quantity, const QString& amount)
{
    QString result;
    QString barcodeTemplate = ProductDBTable::isPiece(product) ?
                appManager->settings->getStringValue(SettingCode_PrintLabelBarcodePiece) :
                appManager->settings->getStringValue(SettingCode_PrintLabelBarcodeWeight);
    if(barcodeTemplate.contains("P"))
    {
        result = ProductDBTable::isPiece(product) ?
                 appManager->settings->getStringValue(SettingCode_PrintLabelPrefixPiece) :
                 appManager->settings->getStringValue(SettingCode_PrintLabelPrefixWeight);
    }
    result += parseBarcode(barcodeTemplate, 'C', product[ProductDBTable::Code].toString()) +
              parseBarcode(barcodeTemplate, 'T', amount) +
              parseBarcode(barcodeTemplate, 'B', product[ProductDBTable::Barcode].toString()) +
              parseBarcode(barcodeTemplate, 'W', quantity);
    Tools::debugLog(QString("@@@@@ EquipmentManager::makeBarcode %1 %2").arg(barcodeTemplate, result));
    return result.length() != barcodeTemplate.length() ? "" : result;
}

QString EquipmentManager::getWMDescription()
{
    QString result;
    if(isWMStarted)
        result = getWMDescriptionNow();
    else
    {
        startWM();
        result = getWMDescriptionNow();
        stopWM();
    }
    return result;
}

QString EquipmentManager::getWMDescriptionNow()
{
    Wm100Protocol::channel_specs cp;
    wm->getChannelParam(&cp);
    QString res, max, min, dis, tar;
    max = "Max ";
    int i = 0;
    while (i < 3 && cp.ranges[i])
    {
        max += QString("%1/").arg(cp.ranges[i]);
        ++i;
    }
    max += QString("%1 кг").arg(cp.max);
    min = QString("Min %1 г").arg(cp.min*1000);
    dis = "e=";
    i = 0;
    while (i < 4 && cp.discreteness[i])
    {
        if (i) dis += "/";
        dis += QString("%1").arg(cp.discreteness[i]*1000);
        ++i;
    }
    dis += " г";
    tar = QString("T=-%1 кг").arg(cp.tare);
    res = QString("%1   %2   %3   %4").arg(max, min, dis, tar);
    return res;
}

int EquipmentManager::print(DataBase* db,
                            const DBRecord& user,
                            const DBRecord& product,
                            const QString& labelLocalPath)
{
    Tools::debugLog("@@@@@ EquipmentManager::print " + labelLocalPath);
    if(!isPMStarted || slpa == nullptr) return Error_PM_Fail;

    quint64 dateTime = Tools::nowMsec();
    int labelNumber = 0; // todo
    int e = 0;

    PrintData pd;
    pd.weight = appManager->status.quantity;
    pd.price = appManager->status.price;
    pd.cost = appManager->status.amount;
    pd.tare = appManager->status.tare;

    QString labelFullPath;
    if(Tools::isFileExistsInDownloadPath(labelLocalPath)) labelFullPath = Tools::downloadPath(labelLocalPath);
    else if(Tools::isFileExists(labelLocalPath)) labelFullPath = labelLocalPath;
    if(labelPath != labelFullPath)
    {
        e = labelCreator->loadLabel(labelFullPath);
        if(e == 0) labelPath = labelFullPath;
    }

    if (e == 0)
    {
        if(appManager->isProduct())
        {
            pd.barcode = makeBarcode(product, pd.weight, pd.cost);
            if (pd.barcode.isEmpty()) e = Error_PM_Barcode;

            pd.itemcode = product[ProductDBTable::Code].toString();
            pd.name = product[ProductDBTable::Name].toString();
            pd.shelflife = product[ProductDBTable::ShelfLife].toString();
            pd.price2 = appManager->calculator->priceAsString(product, ProductDBTable::Price2);
            pd.certificate = product[ProductDBTable::Certificate].toString();
            pd.message = db->getProductMessageById(product[ProductDBTable::MessageCode].toString());
            pd.validity = ""; // todo
        }
        else
        {
            pd.barcode = "";
            pd.itemcode = "";
            pd.name = "";
            pd.shelflife = "";
            pd.price2 = "";
            pd.certificate = "";
            pd.message = "";
            pd.validity = "";
        }
        pd.shop = appManager->settings->getStringValue(SettingCode_ShopName);
        pd.operatorcode =  Tools::toString(Users::getCode(user));
        pd.operatorname = Users::getName(user);
        pd.date = Tools::dateFromUInt(dateTime, DATE_FORMAT);
        pd.time = Tools::timeFromUInt(dateTime, TIME_FORMAT);
        pd.labelnumber = QString::number(labelNumber);
        pd.scalesnumber = appManager->settings->getStringValue(SettingCode_ScalesNumber),
        pd.picturefile = ""; // todo
        pd.textfile = ""; // todo
    }
    if(e == 0)
    {
        QImage p = labelCreator->createImage(pd);
        e = slpa->print(p);
    }
    if(e == 0)
    {
        TransactionDBTable* t = (TransactionDBTable*)(db->getTable(DBTABLENAME_TRANSACTIONS));
        if(t != nullptr)
        {
            DBRecord r = t->createRecord(
                        dateTime,
                        Users::getCode(user),
                        appManager->isProduct() ? Tools::toInt(product[ProductDBTable::Code]) : 0,
                        labelNumber,
                        Tools::toDouble(pd.weight),
                        Tools::toInt(pd.price),
                        Tools::toInt(pd.cost));
            emit printed(r);
        }
        else Tools::debugLog("@@@@@ EquipmentManager::print ERROR (get Transactions Table)");
    }
    if(e != 0) showAttention("Ошибка печати " + getPMErrorDescription(e));
    else if(getPMMode() == EquipmentMode_Demo) showAttention("Демо-печать");
    return e;
}

int EquipmentManager::setExternalDisplay(const DBRecord& product)
{
    Wm100Protocol::display_data dd;
    dd.weight = appManager->status.quantity;
    dd.price = appManager->status.price;
    dd.cost = appManager->status.amount;
    dd.tare = appManager->status.tare;
    dd.flZero = isZeroFlag();
    dd.flTare = isTareFlag();
    dd.flCalm = isWeightFixed();
    dd.flAuto = appManager->status.autoPrintMode == AutoPrintMode_On;
    dd.flLock = appManager->isAuthorizationOpened();
    dd.flTools = appManager->isSettingsOpened();
    dd.flDataExchange = appManager->status.isNet;
    if(appManager->isProduct())
    {
        dd.text = product[ProductDBTable::Name].toString();
        dd.flPieces = ProductDBTable::isPiece(product);
        dd.flPer100g = !dd.flPieces && ProductDBTable::is100gBase(product);
        dd.flPerKg = !dd.flPieces && !dd.flPer100g;
    }
    else
    {
        dd.text = "";
        dd.flPieces = dd.flPer100g = dd.flPerKg = false;
    }
    Tools::debugLog(QString("@@@@@ EquipmentManager::setExternalDisplay %1 %2 %3 %4").arg(
                        dd.weight, dd.price, dd.cost, dd.tare));
    return wm->setDisplayData(dd);
}

