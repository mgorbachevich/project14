#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include <QtConcurrent/QtConcurrent>
#include "settingitemlistmodel.h"
#include "appmanager.h"
#include "resourcedbtable.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productpanelmodel.h"
#include "tablepanelmodel.h"
#include "usernamemodel.h"
#include "showcasepanelmodel2.h"
#include "searchpanelmodel.h"
#include "inputproductcodepanelmodel.h"
#include "settingspanelmodel.h"
#include "viewlogpanelmodel.h"
#include "searchfiltermodel.h"
#include "tools.h"
#include "appinfo.h"
#include "weightmanager.h"
#include "printmanager.h"
#include "netserver.h"
#include "screenmanager.h"
#include "keyemitter.h"
#include "settingdbtable.h"

AppManager::AppManager(QQmlContext* qmlContext, const QSize& screenSize, QApplication *application):
    QObject(application), context(qmlContext)
{
    Tools::removeDebugLog();
    debugLog(QString("@@@@@ AppManager::AppManager %1").arg(APP_VERSION));

    ScreenManager* screenManager = new ScreenManager(screenSize);
    context->setContextProperty("screenManager", screenManager);

    KeyEmitter* keyEmitter = new KeyEmitter(this);
    context->setContextProperty("keyEmitter", keyEmitter);

    if(CREATE_DEFAULT_DATA_ON_START && !Tools::isFileExists(DB_PRODUCT_NAME)) createDefaultData();

    db = new DataBase(settings, this);
    user = UserDBTable::defaultAdmin();
    netServer = new NetServer(this, db);
    weightManager = new WeightManager(this, WM_DEMO);
    printManager = new PrintManager(this, db, settings, PRINTER_DEMO);
    timer = new QTimer(this);

    // Versions:
    appInfo.appVersion = APP_VERSION;
    appInfo.dbVersion = db->version();
    appInfo.weightManagerVersion = weightManager->version();
    appInfo.printManagerVersion = printManager->version();
    appInfo.netServerVersion = netServer->version();
    appInfo.ip = Tools::getNetParams().localHostIP;

    connect(this, &AppManager::start, db, &DataBase::onStart);
    connect(netServer, &NetServer::action, this, &AppManager::onNetAction);
    connect(db, &DataBase::started, this, &AppManager::onDBStarted);
    connect(db, &DataBase::showMessage, this, &AppManager::onShowMessage);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(weightManager, &WeightManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
    connect(printManager, &PrintManager::printed, this, &AppManager::onPrinted);
    connect(printManager, &PrintManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
    connect(keyEmitter, &KeyEmitter::enterChar, this, &AppManager::onEnterChar);
    connect(keyEmitter, &KeyEmitter::enterKey, this, &AppManager::onEnterKey);
    connect(timer, &QTimer::timeout, this, &AppManager::onTimer);

    productPanelModel = new ProductPanelModel(this);
    showcasePanelModel = new ShowcasePanelModel2(this);
    tablePanelModel = new TablePanelModel(this);
    settingsPanelModel = new SettingsPanelModel(this);
    searchPanelModel = new SearchPanelModel(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    viewLogPanelModel = new ViewLogPanelModel(this);
    settingItemListModel = new SettingItemListModel(this);
    inputProductCodePanelModel = new InputProductCodePanelModel(this);

    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("tablePanelModel", tablePanelModel);
    context->setContextProperty("settingsPanelModel", settingsPanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);
    context->setContextProperty("viewLogPanelModel", viewLogPanelModel);
    context->setContextProperty("settingItemListModel", settingItemListModel);
    context->setContextProperty("inputProductCodePanelModel", inputProductCodePanelModel);

    onUserAction();
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, &AppManager::start);
    debugLog("@@@@@ AppManager::AppManager Done");
}

void AppManager::onDBStarted()
{
    //showToast("", "Инициализация");
    debugLog("@@@@@ AppManager::onDBStarted");
    onUserAction();
    db->select(DBSelector_UpdateSettingsOnStart, "");
    timer->start(APP_TIMER_MSEC);
    debugLog("@@@@@ AppManager::onDBStarted Done");
}

void AppManager::onNetAction(const int action)
{
    debugLog("@@@@@ AppManager::onNetAction " + QString::number( action));
    netActionTime = Tools::currentDateTimeToUInt();
    switch (action)
    {
    case NetAction_Upload:
        netRoutes++;
        break;
    case NetAction_Delete:
    case NetAction_Download:
        netRoutes++;
        isRefreshNeeded = true;
        break;
    case NetAction_UploadFinished:
    case NetAction_DeleteFinished:
    case NetAction_DownloadFinished:
        netRoutes--;
        break;
    }
}

void AppManager::onTimer()
{
    if(DEBUG_ONTIMER_MESSAGE) debugLog("@@@@@ AppManager::onTimer " +
                                       Tools::intToString((int)(userActionTime / 1000)));
    if(DEBUG_MEMORY_MESSAGE) Tools::debugMemory();
    const quint64 now = Tools::currentDateTimeToUInt();

    if(isAuthorizationOpened()) // Авторизация
    {
        updateSystemStatus();
    }
    else if(isSettingsOpened()) // Настройки
    {}
    else
    {
        // Сброс товара при бездействии:
        if (isProduct() && settings.getIntValue(SettingCode_ProductReset, true) == ProductReset_Time)
        {
            quint64 waitReset = settings.getIntValue(SettingCode_ProductResetTime); // секунды
            if(waitReset > 0 && waitReset * 1000 < now - userActionTime)
            {
                debugLog("@@@@@ AppManager::onTimer reset product");
                resetProduct();
            }
        }

        // Блокировка:
        quint64 waitBlocking = settings.getIntValue(SettingCode_Blocking); // минуты
        if(waitBlocking > 0 && waitBlocking * 1000 * 60 < now - userActionTime)
        {
            debugLog("@@@@@ AppManager::onTimer blocking");
            userActionTime = now;
            resetProduct();
            startAuthorization();
        }
        else updateWeightStatus();
    }

    // Ожидание окончания сетевых запросов:
    if(netServer->isStarted() && netActionTime > 0 && netRoutes == 0 && WAIT_NET_ACTION_MSEC < now - netActionTime)
    {
        debugLog("@@@@@ AppManager::onTimer netActionTime");
        netActionTime = 0;
        if(isRefreshNeeded)
        {
            debugLog("@@@@@ AppManager::onTimer isRefreshNeeded");
            isRefreshNeeded = false;
            db->afterNetAction();
            refreshAll();
            resetProduct();
            showMessage("ВНИМАНИЕ!", "Товары обновлены!");
            // if(!product.isEmpty()) db->selectByParam(DataBase::RefreshCurrentProduct, product.at(ProductDBTable::Code).toString());
        }
    }
}

QString AppManager::quantityAsString(const DBRecord& productRecord)
{
    if(ProductDBTable::isPiece(productRecord)) return QString("%1").arg(printStatus.pieces);
    return weightManager->isError() ? "" : QString("%1").arg(weightManager->getWeight(), 0, 'f', 3);
}

QString AppManager::priceAsString(const DBRecord& productRecord)
{
    return Tools::moneyToText(price(productRecord), settings.getIntValue(SettingCode_PointPosition));
}

QString AppManager::amountAsString(const DBRecord& productRecord)
{
    double q = 0;
    if(ProductDBTable::isPiece(productRecord))
        q = printStatus.pieces;
    else if(!weightManager->isError())
        q = weightManager->getWeight() * (ProductDBTable::is100gBase(productRecord) ? 10 : 1);
    return Tools::moneyToText(q * price(productRecord), settings.getIntValue(SettingCode_PointPosition));
}

void AppManager::createDefaultData()
{
    debugLog("@@@@@ AppManager::createDefaultData");
    Tools::removeFile(Tools::dbPath(DB_PRODUCT_NAME));
    Tools::removeFile(Tools::dbPath(DB_LOG_NAME));
    Tools::removeFile(Tools::dbPath(DB_SETTINGS_NAME));
    Tools::removeFile(Tools::dbPath(DB_TEMP_NAME));
    Tools::removeFile(Tools::dbPath(DEBUG_LOG_NAME));

    QStringList images = { "1.png", "2.png", "3.jpg", "4.png", "5.png", "6.png", "8.png",
                           "9.png", "10.png", "11.png", "12.png", "15.png" };
    for (int i = 0; i < images.count(); i++)
    {
        Tools::copyFile(QString(":/Default/%1").arg(images[i]),
                        Tools::dbPath(QString("%1/pictures/%2").arg(DOWNLOAD_SUBDIR, images[i])));
    }
    Tools::copyFile(QString(":/Default/%1").arg(DB_PRODUCT_NAME),
                    Tools::dbPath(DB_PRODUCT_NAME));
}

double AppManager::price(const DBRecord& productRecord)
{
    const int p = ProductDBTable::Price;
    if (productRecord.count() <= p) return 0;
    return Tools::priceToDouble(productRecord[p].toString(), settings.getIntValue(SettingCode_PointPosition));
}

void AppManager::onProductDescriptionClicked()
{
    debugLog("@@@@@ AppManager::onProductDescriptionClicked");
    onUserAction();
    db->select(DBSelector_GetMessageByResourceCode, product[ProductDBTable::MessageCode].toString());
}

void AppManager::onProductPanelCloseClicked()
{
    onUserAction();
    resetProduct();
}

void AppManager::onProductPanelPiecesClicked()
{
    onUserAction();
    if(ProductDBTable::isPiece(product)) emit showPiecesInputBox(printStatus.pieces);
    else beepSound();
}

void AppManager::onRewind() // Перемотка
{
    debugLog("@@@@@ AppManager::onRewind ");
    if(isAuthorizationOpened() || isSettingsOpened()) beepSound();
    else printManager->feed();
}

void AppManager::filteredSearch()
{
    // Формирование запросов в БД в зависимости от параметров поиска

    QString& v = searchFilterModel->filterValue;
    debugLog(QString("@@@@@ AppManager::filteredSearch %1 %2").arg(QString::number(searchFilterModel->index), v));
    switch(searchFilterModel->index)
    {
        case SearchFilterModel::Code:
            db->select(DBSelector_GetProductsByFilteredCode, v);
            break;
        case SearchFilterModel::Barcode:
            db->select(DBSelector_GetProductsByFilteredBarcode, v);
            break;
         default:
            debugLog("@@@@@ AppManager::filteredSearch ERROR");
            //emit saveLog(LogType_Warning, LogSource_AppManager, "Неизвестный фильтр поиска");
            break;
    }
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    // Изменился шаблон поиска

    debugLog("@@@@@ AppManager::onSearchFilterEdited " + value);
    searchFilterModel->filterValue = value;
    filteredSearch();
}

void AppManager::onSearchFilterClicked(const int index)
{
    // Изменилось поле поиска (код, штрих-код...)

    debugLog("@@@@@ AppManager::onSearchFilterClicked " + QString::number( index));
    onUserAction();
    searchFilterModel->index = (SearchFilterModel::FilterIndex)index;
    filteredSearch();
}

void AppManager::onTableBackClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    debugLog("@@@@@ AppManager::onTableBackClicked");
    onUserAction();
    if (tablePanelModel->groupUp()) updateTablePanel(false);
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    // Настройка изменилась

    debugLog(QString("@@@@@ AppManager::onSettingInputClosed %1 %2").arg(QString::number(settingItemCode), value));
    DBRecord* r = settings.getByCode(settingItemCode);
    if (r != nullptr && settings.onManualInputValue(settingItemCode, value))
    {
        db->updateSettingsRecord(DBSelector_ReplaceSettingsItem, *r);
        QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(QString::number(settingItemCode), value);
        db->saveLog(LogType_Warning, LogSource_Admin, s);
    }
    else
    {
        debugLog("@@@@@ AppManager::onSettingInputClosed ERROR");
        //showMessage("ВНИМАНИЕ!", "Ошибка при сохранении значения настройки!");
    }
}

void AppManager::onAdminSettingsClicked()
{
    debugLog("@@@@@ AppManager::onAdminSettingsClicked");
    onUserAction();
    startSettings();
}

void AppManager::onLockClicked()
{
    debugLog("@@@@@ AppManager::onLockClicked");
    onUserAction();
    showConfirmation(ConfirmSelector::ConfirmSelector_Authorization, "Подтверждение", "Вы хотите сменить пользователя?");
}

void AppManager::onNumberClicked(const QString &s)
{
    if(!isAuthorizationOpened() && !isSettingsOpened())
    {
        debugLog("@@@@@ AppManager::onNumberClicked " + s);
        emit showProductCodeInputBox(s);
    }
}

void AppManager::onPiecesInputClosed(const QString &value)
{
    debugLog("@@@@@ AppManager::onPiecesInputClosed " + value);
    onUserAction();
    int v = Tools::stringToInt(value);
    if(v < 1)
    {
        v = 1;
        showMessage("ВНИМАНИЕ!", "Количество не должно быть меньше 1");
    }
    printStatus.pieces = v;
    updateWeightStatus();
}

void AppManager::onSetProductByCodeClicked(const QString &value)
{
    debugLog("@@@@@ AppManager::onSetProductByCodeClicked " + value);
    db->select(DBSelector_SetProductByInputCode, value);
}

void AppManager::onProductCodeEdited(const QString &value)
{
    debugLog("@@@@@ AppManager::onProductCodeEdited " + value);
    db->select(DBSelector_GetProductByInputCode, value);
    db->select(DBSelector_GetProductsByInputCode, value);
}

void AppManager::onDBRequestResult(const DBSelector selector, const DBRecordList& records, const bool ok)
{
    // Получен результ из БД

    debugLog("@@@@@ AppManager::onDBRequestResult " + QString::number(selector));
    if (!ok)
    {
        showMessage("ВНИМАНИЕ!", "Ошибка базы данных!");
        return;
    }

    switch(selector)
    {
    case DBSelector_UpdateSettingsOnStart:
        if(!records.isEmpty()) settings.update(records);
        settingsPanelModel->update(settings);
        startAuthorization();
        break;

    case DBSelector_ChangeSettings: // Обновление настроек без перезапуска оборудования:
        if(!records.isEmpty()) settings.update(records);
        settingsPanelModel->update(settings);
        break;

    case DBSelector_ReplaceSettingsItem: // Изменена настройка оператором:
        db->select(DBSelector_ChangeSettings, "");
        break;

    case DBSelector_GetAuthorizationUserByName: // Получен результат поиска пользователя по введеному имени при авторизации:
        stopAuthorization(records);
        break;

    case DBSelector_GetShowcaseProducts: // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        db->select(DBSelector_GetShowcaseResources, records);
        break;

    case DBSelector_GetMessageByResourceCode: // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            showMessage("Описание товара", records[0][ResourceDBTable::Value].toString());
        break;

    case DBSelector_GetAuthorizationUsers: // Отображение имен пользователей при авторизации:
        showUsers(records);
        break;

    case DBSelector_GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DBSelector_GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        tablePanelModel->update(records);
        break;

    case DBSelector_SetProductByInputCode: // Отображение товара с заданным кодом:
        if(records.isEmpty())
            showMessage("", "Товар не найден!");
        else
        {
            emit closeInputProductPanel();
            setProduct(records.at(0));
        }
        break;

    case DBSelector_GetProductByInputCode: // Отображение товара с заданным кодом:
        emit enableSetProductByInputCode(records.count() == 1 && !records.at(0).isEmpty());
        break;

    case DBSelector_GetProductsByInputCode: // Отображение товаров с заданным фрагментом кода:
        inputProductCodePanelModel->update(records);
        break;

    case DBSelector_GetProductsByFilteredCode: // Отображение товаров с заданным фрагментом кода:
        searchPanelModel->update(records, SearchFilterModel::Code);
        break;

    case DBSelector_GetProductsByFilteredBarcode: // Отображение товаров с заданным фрагментом штрих-кода:
        searchPanelModel->update(records, SearchFilterModel::Barcode);
        break;

    case DBSelector_RefreshCurrentProduct: // Сброс выбранного товара после изменений в БД:
        resetProduct();
        if (!records.isEmpty() && !records.at(0).isEmpty())
        {
            //showToast("ВНИМАНИЕ!", "Выбранный товар изменен");
            setProduct(records.at(0));
        }
        break;

    case DBSelector_GetShowcaseResources: // Отображение картинок товаров экрана Showcase:
    {
        debugLog("@@@@@ AppManager::onDBRequestResult GetShowcaseResources"  + QString::number(records.count()));
        QStringList fileNames;
        for (int i = 0; i < records.count(); i++)
        {
            QString s = getImageFileWithQmlPath(records[i]);
            // debugLog("@@@@@ AppManager::onDBRequestResult GetShowcaseResources" << s;
            fileNames.append(s);
        }
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DBSelector_GetImageByResourceCode: // Отображение картинки выбранного товара:
    {
        QString imagePath = records.count() > 0 ? getImageFileWithQmlPath(records[0]) : DUMMY_IMAGE_FILE;
        emit showProductImage(imagePath);
        debugLog("@@@@@ AppManager::onDBRequestResult showProductImage " + imagePath);
        //showMessage("Image file path", imagePath);
        break;
    }

    default:break;
    }
}

QString AppManager::getImageFileWithQmlPath(const DBRecord& r)
{
    QString path = DUMMY_IMAGE_FILE;
    const int i = ResourceDBTable::Value;
    if (r.count() > i)
    {
        QString localFilePath = r[i].toString();
        if(Tools::isFileExistsInDownloadPath(localFilePath))
            path = Tools::qmlFilePath(localFilePath);
    }
    return path;
}

void AppManager::onViewLogClicked()
{
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр лога");
    onUserAction();
    db->select(DBSelector_GetLog, "");
    emit showViewLogPanel();
}

void AppManager::onVirtualKeyboardSet(const int v)
{
    debugLog("@@@@@ AppManager::onVirtualKeyboardSet " + Tools::intToString(v));
    onUserAction();
    emit showVirtualKeyboard(v);
}

void AppManager::onWeightPanelClicked(const int param)
{
    debugLog("@@@@@ AppManager::onWeightPanelClicked " + Tools::intToString(param));
    if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, &AppManager::onUserAction);
    if(param == secret + 1 && (++secret) == 3) onLockClicked();
}

void AppManager::onTareClicked()
{
    debugLog("@@@@@ AppManager::onTareClicked ");
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Tare);
    updateWeightStatus();
}

void AppManager::onZeroClicked()
{
    debugLog("@@@@@ AppManager::onZeroClicked ");
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Zero);
    updateWeightStatus();
}

void AppManager::onConfirmationClicked(const int selector)
{
    debugLog("@@@@@ AppManager::onConfirmationClicked " + Tools::intToString(selector));
    onUserAction();
    switch (selector)
    {
    case ConfirmSelector_Authorization:
        startAuthorization();
        break;
    case ConfirmSelector_ClearLog:
        emit closeLogView();
        db->clearLog();
        break;
    case ConfirmSelector_SetSystemDateTime:
        weightManager->setSystemDateTime = true;
        break;
    }
}

void AppManager::onInfoClicked()
{
    debugLog("@@@@@ AppManager::onInfoClicked ");
    onUserAction();
    showMessage("Инфо", appInfo.all(EOL));
}

void AppManager::onTableResultClicked(const int index)
{
    debugLog("@@@@@ AppManager::onTableResultClicked " + Tools::intToString(index));
    onUserAction();
    if (index < tablePanelModel->productCount())
    {
        DBRecord& clickedProduct = tablePanelModel->productByIndex(index);
        if (ProductDBTable::isGroup(clickedProduct))
        {
            if (tablePanelModel->groupDown(clickedProduct)) updateTablePanel(false);
        }
        else setProduct(clickedProduct);
    }
}

void AppManager::onSettingsItemClicked(const int index)
{
    onUserAction();
    DBRecord* r = settings.getByIndexInCurrentGroup(index);
    if(r == nullptr || r->empty())
    {
        debugLog("@@@@@ AppManager::onSettingsItemClicked ERROR " + Tools::intToString(index));
        return;
    }

    const int code = settings.getCode(*r);
    const QString& name = settings.getName(*r);
    const int type = settings.getType(*r);
    debugLog(QString("@@@@@ AppManager::onSettingsItemClicked %1 %2 %3").arg(Tools::intToString(code), name, QString::number(type)));

    switch (type)
    {
    case SettingType_Group:
        settings.currentGroupCode = code;
        settingsPanelModel->update(settings);
        emit showSettingsPanel(settings.getCurrentGroupName());
        break;
    case SettingType_InputNumber:
    case SettingType_InputText:
        emit showSettingInputBox(code, name, settings.getStringValue(*r));
        break;
    case SettingType_List:
        settingItemListModel->update(settings.getValueList(*r));
        emit showSettingComboBox(code, name, settings.getIntValue(*r), settings.getStringValue(*r));
        break;
    case SettingType_IntervalNumber:
    {
        QStringList list = settings.getValueList(*r);
        if(list.count() >= 2)
        {
            int from = Tools::stringToInt(list[0]);
            int to = Tools::stringToInt(list[1]);
            int value = settings.getIntValue(*r);
            emit showSettingSlider(code, name, from, to, 1, value);
        }
        break;
    }
    case SettingType_Custom:
        onCustomSettingsItemClicked(*r);
        break;
    case SettingType_Unsed:
        showMessage(name, "Не поддерживается");
        break;
    case SettingType_ReadOnly:
        showMessage(name, "Редактирование запрещено");
        break;
    default:
        break;
    }
}

void AppManager::clearLog()
{
    debugLog("@@@@@ AppManager::clearLog");
    showConfirmation(ConfirmSelector_ClearLog, "Подтверждение", "Вы хотите очистить лог?");
}

void AppManager::onCustomSettingsItemClicked(const DBRecord& r)
{
    const int code = settings.getCode(r);
    const QString& name = settings.getName(r);
    debugLog(QString("@@@@@ AppManager::onCustomSettingsItemClicked %1 %2").arg(Tools::intToString(code), name));

    switch (code)
    {
    case SettingCode_ClearLog:
        clearLog();
        break;
    case SettingCode_DateTime:
        inputDateTime();
        break;
    case SettingCode_Equipment:
    case SettingCode_WiFi:
    case SettingCode_SystemSettings:
    case SettingCode_Ethernet:
        switch(settings.nativeSettings(code))
        {
        case 0: // Ошибок нет
            break;
        case -1:
            showMessage(name, "ОШИБКА! Неизвестный параметр");
            break;
        case -2:
            showMessage(name, "ОШИБКА! Неверный вызов");
            break;
        }
        break;
    default:
        showMessage(name, "Не поддерживается");
        return;
    }
}

void AppManager::onSettingsPanelCloseClicked()
{
    debugLog("@@@@@ AppManager::onSettingsPanelCloseClicked " + Tools::intToString(settings.currentGroupCode));
    onUserAction();
    emit previousSettings();
    if(settings.currentGroupCode != 0)
    {
        DBRecord* r = settings.getByCode(settings.currentGroupCode);
        if(r != nullptr && !r->empty() && settings.isGroup(*r))
        {
            // Переход вверх:
            settings.currentGroupCode = r->at(SettingDBTable::GroupCode).toInt();
            settingsPanelModel->update(settings);
            emit showSettingsPanel(settings.getCurrentGroupName());
            return;
        }
    }
    stopSettings();
}

void AppManager::onSearchResultClicked(const int index)
{
    debugLog("@@@@@ AppManager::onSearchResultClicked " + Tools::intToString(index));
    onUserAction();
    setProduct(searchPanelModel->productByIndex(index));
}

void AppManager::onShowcaseClicked(const int index)
{
    debugLog("@@@@@ AppManager::onShowcaseClicked " + Tools::intToString(index));
    onUserAction();
    setProduct(showcasePanelModel->productByIndex(index));
}

void AppManager::onShowcaseSortClicked(const int sort)
{
    debugLog("@@@@@ AppManager::onShowcaseSortClicked " + Tools::intToString(sort));
    onUserAction();
    setShowcaseSort(sort);
}

void AppManager::onMainPageSwiped(const int i)
{
    debugLog("@@@@@ AppManager::onMainPageSwiped " + Tools::intToString(i));
    setMainPage(i);
}

void AppManager::setMainPage(const int i)
{
    debugLog("@@@@@ AppManager::setMainPage " + Tools::intToString(i));
    mainPageIndex = i;
    emit showMainPage(mainPageIndex);
}

void AppManager::updateTablePanel(const bool root)
{
    debugLog("@@@@@ AppManager::updateTablePanel");
    if (root) tablePanelModel->root();
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    db->select(DBSelector_GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    onUserAction();
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    debugLog(QString("@@@@@ AppManager::onCheckAuthorizationClick %1 %2").arg(normalizedLogin, password));
    user[UserDBTable::Name] = normalizedLogin;
    user[UserDBTable::Password] = password;
    db->select(DBSelector_GetAuthorizationUserByName, normalizedLogin);
}

void AppManager::startSettings()
{
    debugLog("@@@@@ AppManager::startSettings");
    isSettings = true;
    stopEquipment();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    settingsPanelModel->update(settings);
    emit showSettingsPanel(settings.getCurrentGroupName());
}

void AppManager::stopSettings()
{
    debugLog("@@@@@ AppManager::stopSettings");
    setMainPage(mainPageIndex);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::stopSettings pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        startEquipment();
        isSettings = false;
    });
}

void AppManager::startAuthorization()
{
    debugLog("@@@@@ AppManager::startAuthorization");
    stopEquipment();
    setMainPage(-1);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::startAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        updateSystemStatus();
        db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
        db->select(DBSelector_GetAuthorizationUsers, "");
        debugLog("@@@@@ AppManager::startAuthorization Done");
    });
}

void AppManager::stopAuthorization(const DBRecordList& dbUsers)
{
    // Введены логин и пароль. Проверка
    const QString title = "Авторизация";
    QString login = "";

    if(!CHECK_AUTHORIZATION) // Без проверки
    {
        user = UserDBTable::defaultAdmin();
        login = user[UserDBTable::Name].toString();
        debugLog(QString("@@@@@ AppManager::stopAuthorization %1").arg(login));
    }
    else
    {
        login = user[UserDBTable::Name].toString();
        const QString password = user[UserDBTable::Password].toString();
        debugLog(QString("@@@@@ AppManager::stopAuthorization %1 %2").arg(login, password));
        if (!dbUsers.isEmpty())
        {
            if (login != dbUsers[0][UserDBTable::Name].toString() || password != dbUsers[0][UserDBTable::Password])
            {
                debugLog("@@@@@ AppManager::stopAuthorization ERROR");
                const QString error = "Неверные имя пользователя или пароль";
                showMessage(title, error);
                db->saveLog(LogType_Warning, LogSource_User, QString("%1. %2").arg(title, error));
                onUserAction();
                return;
            }
            user.clear();
            user.append(dbUsers[0]);
        }
    }

    debugLog("@@@@@ AppManager::stopAuthorization OK");
    QString s = QString("%1. Пользователь: %2. Код: %3").arg(title, login, user[UserDBTable::Code].toString());
    db->saveLog(LogType_Warning, LogSource_User, s);
    setMainPage(0);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::stopAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        startEquipment();
        refreshAll();
        resetProduct();
        updateWeightStatus();
        onUserAction();
        if(SHOW_PATH_MESSAGE) showMessage("БД", Tools::dbPath(DB_PRODUCT_NAME));
        debugLog("@@@@@ AppManager::stopAuthorization Done");
    });
}

void AppManager::setShowcaseSort(const int sort)
{
    showcaseSort = sort;
    db->select(DBSelector_GetShowcaseProducts, QString::number(sort));
    emit showShowcaseSort(sort);
}

void AppManager::refreshAll()
{
    // Обновить всё на экране
    debugLog("@@@@@ AppManager::refreshAll");
    emit showAdminMenu(UserDBTable::isAdmin(user));
    setShowcaseSort(showcaseSort);
    searchFilterModel->update();
    //searchPanelModel->update();
    filteredSearch();
    updateTablePanel(true);
}

void AppManager::showToast(const QString &title, const QString &text, const int delaySec)
{
    debugLog(QString("@@@@@ AppManager::showToast %1 %2").arg(title, text));
    emit showMessageBox(title, text, false);
    QTimer::singleShot(delaySec * 1000, this, &AppManager::hideToast);
}

void AppManager::updateSystemStatus()
{
    QString dateTime = Tools::dateTimeFromUInt(Tools::currentDateTimeToUInt(), "%1 %2", "dd.MM", "hh:mm");
    debugLog(QString("@@@@@ AppManager::updateSystemStatus %1").arg(dateTime));
    emit showDateTime(dateTime);
    emit showEnvironmentStatus(Tools::isEnvironment(EnvironmentType_USB),
                               Tools::isEnvironment(EnvironmentType_Bluetooth),
                               Tools::isEnvironment(EnvironmentType_WiFi),
                               Tools::isEnvironment(EnvironmentType_SDCard));
}

void AppManager::showMessage(const QString &title, const QString &text)
{
    debugLog(QString("@@@@@ AppManager::showMessage %1 %2").arg(title, text));
    emit showMessageBox(title, text, true);
}

void AppManager::showConfirmation(const ConfirmSelector selector, const QString &title, const QString &text)
{
    debugLog(QString("@@@@@ AppManager::showConfirmation %1 %2 %3").arg(QString::number(selector), title, text));
    emit showConfirmationBox(selector, title, text);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    if(!product.isEmpty())
    {
        QString productCode = product[ProductDBTable::Code].toString();
        debugLog("@@@@@ AppManager::setProduct " + productCode);
        productPanelModel->update(product, price(product), (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
        emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
        db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
        QString pictureCode = product[ProductDBTable::PictureCode].toString();
        db->select(DBSelector_GetImageByResourceCode, pictureCode);
        printStatus.onNewProduct();
        updateWeightStatus();
    }
}

void AppManager::resetProduct() // Сбросить выбранный продукт
{
    isResetProductNeeded = false;
    if(isProduct())
    {
        debugLog("@@@@@ AppManager::resetProduct");
        product.clear();
        printStatus.onResetProduct();
        emit resetCurrentProduct();
        updateWeightStatus();
    }
}

void AppManager::startEquipment()
{
    debugLog("@@@@@ AppManager::startEquipment");

    const int serverPort = settings.getIntValue(SettingCode_TCPPort);
    debugLog("@@@@@ AppManager::startEquipment serverPort = " + QString::number(serverPort));
    netServer->start(serverPort);

    QString message;
    EquipmentUris eu;
    QString defaultFileName = ":/Text/json_default_equipment_config.txt";

#ifdef Q_OS_ANDROID
    if(Tools::checkPermission("android.permission.READ_EXTERNAL_STORAGE"))
    {
        QString configFileName = "/mnt/sdcard/shtrihm/json_settingsfile.txt";
        eu = settings.parseEquipmentConfig(configFileName);
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ read config file =\n" + configFileName);
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ WMUri =\n" + eu.wmUri);
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ PrinterUri =\n" + eu.printerUri);
    }
    else
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ read config file ERROR = no permission");

    if(eu.wmUri.isEmpty() || eu.printerUri.isEmpty())
    {
        message += "\nОшибка чтения конфигурационного файла ";
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ read config file ERROR");

        eu = settings.parseEquipmentConfig(defaultFileName); //  По умолчанию
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ read default file =\n" +  defaultFileName);
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ WMUri =\n" + eu.wmUri);
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ PrinterUri =\n" + eu.printerUri);

        if(eu.wmUri.isEmpty() || eu.printerUri.isEmpty())
        {
            message += "\nОшибка чтения конфигурационного файла по умолчанию ";
            db->saveLog(LogType_Info, LogSource_User, "@@@@@ read default file ERROR");
        }
        else
        {
            message += "\nКонфигурация по умолчанию ";
            db->saveLog(LogType_Info, LogSource_User, "@@@@@ set default config");
        }
    }
#else
    eu = settings.parseEquipmentConfig(defaultFileName); //  По умолчанию
    if(eu.wmUri.isEmpty() || eu.printerUri.isEmpty())
        message += "\nОшибка чтения конфигурационного файла по умолчанию ";
#endif // Q_OS_ANDROID

    if(WM_DEMO || eu.wmUri.contains("demo", Qt::CaseInsensitive)) message += "\nДемо-режим весового модуля";
    int e = weightManager->start(eu.wmUri);
    db->saveLog(LogType_Info, LogSource_User, "@@@@@ start WM error = " + Tools::intToString(e));
    if(e) message += QString("\nОшибка весового модуля %1: %2").arg(Tools::intToString(e), weightManager->getErrorDescription(e));

    if(PRINTER_DEMO || eu.printerUri.contains("demo", Qt::CaseInsensitive)) message += "\nДемо-режим принтера";
    e = printManager->start(eu.printerUri);
    db->saveLog(LogType_Info, LogSource_User, "@@@@@ start printer error = " + Tools::intToString(e));
    if(e) message += QString("\nОшибка принтера %1: %2").arg(QString::number(e), printManager->getErrorDescription(e));
    else
    {
        e = printManager->setParams(settings.getIntValue(SettingCode_PrinterBrightness),
                                    settings.getIntValue(SettingCode_PrintOffset));
        db->saveLog(LogType_Info, LogSource_User, "@@@@@ set printer params error = " + Tools::intToString(e));
        if(e) message += QString("\nОшибка установки параметров принтера %1: %2").arg(Tools::intToString(e), printManager->getErrorDescription(e));
    }

    if(!message.isEmpty()) showMessage("ВНИМАНИЕ!", message);

    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::startEquipment pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        timer->blockSignals(false);
        debugLog("@@@@@ AppManager::startEquipment Done");
    });
}

void AppManager::stopEquipment(const bool server, const bool weight, const bool printer)
{
    debugLog("@@@@@ AppManager::stopEquipment");
    timer->blockSignals(true);
    if (server) netServer->stop();
    if (weight) weightManager->stop();
    if (printer) printManager->stop();
    debugLog("@@@@@ AppManager::stopEquipment Done");
}

void AppManager::onUserAction()
{
    debugLog("@@@@@ AppManager::onUserAction");
    if(DEBUG_MEMORY_MESSAGE) Tools::debugMemory();
    userActionTime = Tools::currentDateTimeToUInt();
    secret = 0;
}

void AppManager::showUsers(const DBRecordList& records)
{
    // Обновить список пользователей на экране авторизации

    DBRecordList users;
    if (records.isEmpty()) // В базе нет пользователей. Добавить администратора по умолчанию:
        users << UserDBTable::defaultAdmin();
    else
        users.append(records);
    userNameModel->update(users);

    if(users.count() == 1)
    {
        user.clear();
        user.append(users.at(0));
        emit setCurrentUser(0, user.at(UserDBTable::Name).toString());
    }
    else for (int i = 0; i < users.count(); i++)
    {
        if(users.at(i).at(UserDBTable::Code).toInt() == user.at(UserDBTable::Code).toInt())
        {
            emit setCurrentUser(i, users.at(i).at(UserDBTable::Name).toString());
            break;
        }
    }
}

void AppManager::print() // Печатаем этикетку
{
    debugLog("@@@@@ AppManager::print ");
    printManager->print(user, product, quantityAsString(product), priceAsString(product), amountAsString(product));
}

void AppManager::onPrintClicked()
{
    debugLog("@@@@@ AppManager::onPrintClicked ");
    onUserAction();
    if(printStatus.manualPrintEnabled)
    {
        printStatus.calculateMode = false;
        print();
    }
}

void AppManager::onPrinted(const DBRecord& newTransaction)
{
    // Принтер ответил что этикетка напечатана

    debugLog("@@@@@ AppManager::onPrinted");
    db->saveLog(LogType_Error, LogSource_Print, QString("Печать. Код товара: %1. Вес/Количество: %2").arg(
                newTransaction[TransactionDBTable::ItemCode].toString(),
                newTransaction[TransactionDBTable::Weight].toString()));
    db->saveTransaction(newTransaction);
    if (settings.getIntValue(SettingCode_ProductReset, true) == ProductReset_Print) isResetProductNeeded = true;
}

void AppManager::onEquipmentParamChanged(const int param, const int errorCode)
{
    // Изменился параметр оборудования
    debugLog(QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(
                 Tools::intToString(param), Tools::intToString(errorCode)));

    switch (param)
    {
    case EquipmentParam_None:
        return;
    case EquipmentParam_WeightValue:
        if(isResetProductNeeded) resetProduct();
        break;
    case EquipmentParam_WeightError:
        if(isResetProductNeeded) resetProduct();
        db->saveLog(LogType_Error, LogSource_Weight, QString("Ошибка весового модуля. Код: %1. Описание: %2").arg(
                    QString::number(errorCode),
                    weightManager->getErrorDescription(errorCode)));
        break;
    case EquipmentParam_PrintError:
        db->saveLog(LogType_Error, LogSource_Print, QString("Ошибка принтера. Код: %1. Описание: %2").arg(
                    QString::number(errorCode),
                    printManager->getErrorDescription(errorCode)));
        emit showPrinterMessage(printManager->getErrorDescription(errorCode));
        break;
    }
    updateWeightStatus();
}

void AppManager::updateWeightStatus()
{
    debugLog("@@@@@ AppManager::updateWeightStatus");

    const bool isPieceProduct = isProduct() && ProductDBTable::isPiece(product);
    const int oldPieces = printStatus.pieces;
    if(isPieceProduct && printStatus.pieces < 1) printStatus.pieces = 1;

    const bool isZero = weightManager->isZeroFlag();
    const bool isTare = weightManager->isTareFlag();
    const bool isWeightError = weightManager->isError();
    const bool isPrintError = printManager->isError();
    const bool isFixed = weightManager->isWeightFixed();

    const bool isAutoPrint = settings.getBoolValue(SettingCode_PrintAuto) &&
           (!isPieceProduct || settings.getBoolValue(SettingCode_PrintAutoPcs));
    const QString passiveColor = "#424242";
    const QString activeColor = "#fafafa";

    // Проверка флага 0 - новый товар на весах (начинать обязательно с этого!):
    if(isZero) printStatus.calculateMode = true;

    // Рисуем флажки:
    emit showWeightParam(EquipmentParam_AutoPrint, QString::number(isAutoPrint ? AutoPrint_Enable : AutoPrint_None));
    emit showWeightParam(EquipmentParam_WeightError, Tools::boolToString(isWeightError));
    emit showWeightParam(EquipmentParam_Zero, Tools::boolToString(isZero));
    emit showWeightParam(EquipmentParam_Tare, Tools::boolToString(isTare));

    // Рисуем загаловки:
    QString pt = "ЦЕНА, руб";
    if(isProduct())
    {
        if (isPieceProduct) pt += "/шт";
        else pt += ProductDBTable::is100gBase(product) ? "/100г" : "/кг";
    }
    emit showWeightParam(EquipmentParam_WeightTitle, isPieceProduct ? "КОЛИЧЕСТВО, шт" : "МАССА, кг");
    emit showWeightParam(EquipmentParam_PriceTitle, pt);
    emit showWeightParam(EquipmentParam_AmountTitle, "СТОИМОСТЬ, руб");

    // Рисуем количество (вес/штуки):
    const QString quantity = isPieceProduct || !isWeightError ? quantityAsString(product) : "";
    emit showWeightParam(EquipmentParam_WeightValue, quantity);
    emit showWeightParam(EquipmentParam_WeightColor, isPieceProduct || (isFixed && !isWeightError) ? activeColor : passiveColor);

    // Рисуем цену:
    const QString price = isProduct() ? priceAsString(product) : "-----";
    emit showWeightParam(EquipmentParam_PriceValue, price);
    emit showWeightParam(EquipmentParam_PriceColor, isProduct() ? activeColor : passiveColor);

    // Рисуем стоимость:
    const bool isAmount = isPieceProduct || (isProduct() && isFixed && !isWeightError);
    const QString amount = amountAsString(product);
    emit showWeightParam(EquipmentParam_AmountValue, amount);
    emit showWeightParam(EquipmentParam_AmountColor, isAmount ? activeColor : passiveColor);

    // Можно печатать?
    if(printManager->isDemoMode()) emit showPrinterMessage("Демо режим принтера");
    printStatus.manualPrintEnabled = isAmount && !isPrintError;
    emit enableManualPrint(printStatus.manualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && printStatus.manualPrintEnabled;

    if(printStatus.calculateMode && isProduct())
    {
        if(isWeightError || isFixed || weightManager->isDemoMode()) printStatus.calculateMode = false;

        if(!isPieceProduct) // Весовой товар
        {
            if(isFixed && !isWeightError && isAutoPrintEnabled) // Автопечать
            {
                const int wg = (int)(weightManager->getWeight() * 1000); // Вес в граммах
                if(wg > 0)
                {
                    if(wg >= settings.getIntValue(SettingCode_PrintAutoWeight)) print();
                    else showToast("ВНИМАНИЕ!", "Вес слишком мал для автопечати");
                }
            }
        }
        else // Штучный товар
        {
            const double unitWeight = product[ProductDBTable::UnitWeight].toDouble() / 1000000; // Вес единицы, кг
            if(unitWeight > 0) // Задан вес единицы
            {
                if(isFixed && !isWeightError)
                {
                    int newPieces = (int)(weightManager->getWeight() / unitWeight + 0.5); // Округление до ближайшего целого
                    printStatus.pieces = newPieces < 1 ? 1 : newPieces;
                    if(isAutoPrintEnabled) print(); // Автопечать
                }
            }
            else
            {
                printStatus.calculateMode = false;
                if(isAutoPrintEnabled) print(); // Автопечать
            }
        }
    }
    if(isPieceProduct && oldPieces != printStatus.pieces) updateWeightStatus();
    else debugLog(QString("@@@@@ AppManager::updateWeightStatus %1 %2 %3 %4 %5 %6 %7").arg(
                        Tools::boolToString(isWeightError),
                        Tools::boolToString(isAutoPrint),
                        Tools::boolToString(isTare),
                        Tools::boolToString(isZero),
                        quantity, price, amount));
}

void AppManager::beepSound()
{
    Tools::sound("qrc:/Sound/KeypressInvalid.mp3", settings.getIntValue(SettingCode_KeyboardSoundVolume));
}

void AppManager::clickSound()
{
    Tools::sound("qrc:/Sound/KeypressStandard.mp3", settings.getIntValue(SettingCode_KeyboardSoundVolume));
}

void AppManager::debugLog(const QString& s)
{
    Tools::debugLog(s);
}

void AppManager::inputDateTime()
{
    debugLog("@@@@@ AppManager::inputDateTime");
    showConfirmation(ConfirmSelector_SetSystemDateTime, "Подтверждение", "Вы хотите установить системное время?");
}

void AppManager::onPopupOpened(const bool open)
{
    debugLog(QString("@@@@@ AppManager::onPopupOpened %1 %2").arg(Tools::boolToString(open), Tools::intToString(popups)));
    if(open) popups++;
    else if((--popups) < 1)
    {
        popups = 0;
        if(!isSettingsOpened()) setMainPage(mainPageIndex);
    }
}













