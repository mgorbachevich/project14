#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include <QtConcurrent/QtConcurrent>
#include "SettingItemListModel.h"
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
    Tools::debugLog("@@@@@ AppManager::AppManager");

    ScreenManager* screenManager = new ScreenManager(screenSize);
    context->setContextProperty("screenManager", screenManager);

    KeyEmitter* keyEmitter = new KeyEmitter(this);
    context->setContextProperty("keyEmitter", keyEmitter);

    db = new DataBase(settings, this);
    user = UserDBTable::defaultAdmin();
    netServer = new NetServer(this, db);
    weightManager = new WeightManager(this, WM_DEMO);
    printManager = new PrintManager(this, db, settings, PRINTER_DEMO);
    QTimer *timer = new QTimer(this);

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

    QTimer::singleShot(100, this, &AppManager::start);
    onUserAction();
    timer->start(APP_TIMER_MSEC);

    Tools::debugLog("@@@@@ AppManager::AppManager Done");
}

void AppManager::onDBStarted()
{
    //showToast("", "Инициализация");
    Tools::debugLog("@@@@@ AppManager::onDBStarted");
    onUserAction();
    db->select(DBSelector_UpdateSettingsOnStart, "");
    Tools::debugLog("@@@@@ AppManager::onDBStarted Done");
}

void AppManager::onNetAction(const int action)
{
    Tools::debugLog("@@@@@ AppManager::onNetAction " + QString::number( action));
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
    if(DEBUG_ONTIMER_MESSAGE) Tools::debugLog("@@@@@ AppManager::onTimer");
    const quint64 now = Tools::currentDateTimeToUInt();

    // Блокировка:
    quint64 waitBlocking = settings.getItemIntValue(SettingCode_Blocking); // минуты
    if(!isAuthorizationOpened && userActionTime > 0 && waitBlocking > 0 && waitBlocking * 1000 * 60 < now - userActionTime)
    {
        Tools::debugLog("@@@@@ AppManager::onTimer userActionTime");
        userActionTime = 0;
        startAuthorization();
    }

    // Ожидание окончания сетевых запросов:
    if(netActionTime > 0 && netRoutes == 0 && WAIT_NET_ACTION_MSEC < now - netActionTime)
    {
        Tools::debugLog("@@@@@ AppManager::onTimer netActionTime");
        netActionTime = 0;
        if(isRefreshNeeded)
        {
            Tools::debugLog("@@@@@ AppManager::onTimer isRefreshNeeded");
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
    return Tools::moneyToText(price(productRecord), settings.getItemIntValue(SettingCode_PointPosition));
}

QString AppManager::amountAsString(const DBRecord& productRecord)
{
    double q = 0;
    if(ProductDBTable::isPiece(productRecord))
        q = printStatus.pieces;
    else if(!weightManager->isError())
        q = weightManager->getWeight() * (ProductDBTable::is100gBase(productRecord) ? 10 : 1);
    return Tools::moneyToText(q * price(productRecord), settings.getItemIntValue(SettingCode_PointPosition));
}

double AppManager::price(const DBRecord& productRecord)
{
    const int p = ProductDBTable::Price;
    if (productRecord.count() <= p) return 0;
    return Tools::priceToDouble(productRecord[p].toString(), settings.getItemIntValue(SettingCode_PointPosition));
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    QString productCode = product[ProductDBTable::Code].toString();
    Tools::debugLog("@@@@@ AppManager::setProduct " + productCode);
    productPanelModel->update(product, price(product), (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
    emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
    db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
    QString pictureCode = product[ProductDBTable::PictureCode].toString();
    db->select(DBSelector_GetImageByResourceCode, pictureCode);
    printStatus.onNewProduct();
    updateStatus();

    if (settings.getItemIntValue(SettingCode_ProductReset) ==  ProductReset_Time)
    {
        int resetTime = settings.getItemIntValue(SettingCode_ProductResetTime);
        if (resetTime > 0) QTimer::singleShot(resetTime * 1000, this, &AppManager::resetProduct);
    }
}

void AppManager::onProductDescriptionClicked()
{
    Tools::debugLog("@@@@@ AppManager::onProductDescriptionClicked");
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
    Tools::debugLog("@@@@@ AppManager::onRewind ");
    onUserAction();
    printManager->feed();
}

void AppManager::filteredSearch()
{
    // Формирование запросов в БД в зависимости от параметров поиска

    QString& v = searchFilterModel->filterValue;
    Tools::debugLog(QString("@@@@@ AppManager::filteredSearch %1 %2").arg(QString::number(searchFilterModel->index), v));
    switch(searchFilterModel->index)
    {
        case SearchFilterModel::Code:
            db->select(DBSelector_GetProductsByFilteredCode, v);
            break;
        case SearchFilterModel::Barcode:
            db->select(DBSelector_GetProductsByFilteredBarcode, v);
            break;
         default:
            Tools::debugLog("@@@@@ AppManager::filteredSearch ERROR");
            //emit saveLog(LogType_Warning, LogSource_AppManager, "Неизвестный фильтр поиска");
            break;
    }
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    // Изменился шаблон поиска

    Tools::debugLog("@@@@@ AppManager::onSearchFilterEdited " + value);
    searchFilterModel->filterValue = value;
    filteredSearch();
}

void AppManager::onSearchFilterClicked(const int index)
{
    // Изменилось поле поиска (код, штрих-код...)

    Tools::debugLog("@@@@@ AppManager::onSearchFilterClicked " + QString::number( index));
    onUserAction();
    searchFilterModel->index = (SearchFilterModel::FilterIndex)index;
    filteredSearch();
}

void AppManager::onTableBackClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    Tools::debugLog("@@@@@ AppManager::onTableBackClicked");
    onUserAction();
    if (tablePanelModel->groupUp()) updateTablePanel(false);
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    // Настройка изменилась

    Tools::debugLog(QString("@@@@@ AppManager::onSettingInputClosed %1 %2").arg(QString::number(settingItemCode), value));
    DBRecord* r = settings.getItemByCode(settingItemCode);
    if (r != nullptr && settings.onManualInputItemValue(settingItemCode, value))
    {
        db->updateSettingsRecord(DBSelector_ReplaceSettingsItem, *r);
        QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(QString::number(settingItemCode), value);
        db->saveLog(LogType_Warning, LogSource_Admin, s);
    }
    else
    {
        Tools::debugLog("@@@@@ AppManager::onSettingInputClosed ERROR");
        //showMessage("ВНИМАНИЕ!", "Ошибка при сохранении значения настройки!");
    }
}

void AppManager::onAdminSettingsClicked()
{
    Tools::debugLog("@@@@@ AppManager::onAdminSettingsClicked");
    onUserAction();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    settingsPanelModel->update(settings);
    emit showSettingsPanel(settings.getCurrentGroupName());
}

void AppManager::onLockClicked()
{
    Tools::debugLog("@@@@@ AppManager::onLockClicked");
    onUserAction();
    showConfirmation(DialogSelector::DialogSelector_Authorization, "Подтверждение", "Вы хотите сменить пользователя?");
}

void AppManager::onMainPageChanged(const int index)
{
    Tools::debugLog("@@@@@ AppManager::onMainPageChanged " + QString::number( index));
    mainPageIndex = index;
    emit showMainPage(mainPageIndex);
}

void AppManager::onNumberClicked(const QString &s)
{
    Tools::debugLog("@@@@@ AppManager::onNumberClicked " + s);
    emit showProductCodeInputBox(s);
}

void AppManager::onPiecesInputClosed(const QString &value)
{
    Tools::debugLog("@@@@@ AppManager::onPiecesInputClosed " + value);
    onUserAction();
    int v = Tools::stringToInt(value);
    if(v < 1)
    {
        v = 1;
        showMessage("ВНИМАНИЕ!", "Количество не должно быть меньше 1");
    }
    printStatus.pieces = v;
    updateStatus();
}

void AppManager::onSetProductByCodeClicked(const QString &value)
{
    Tools::debugLog("@@@@@ AppManager::onSetProductByCodeClicked " + value);
    db->select(DBSelector_SetProductByInputCode, value);
}

void AppManager::onProductCodeEdited(const QString &value)
{
    Tools::debugLog("@@@@@ AppManager::onProductCodeEdited " + value);
    db->select(DBSelector_GetProductByInputCode, value);
    db->select(DBSelector_GetProductsByInputCode, value);
}

void AppManager::onPopupClosed()
{
    if (--openedPopupCount <= 0)
    {
        openedPopupCount = 0;
        emit showMainPage(mainPageIndex);
        isAuthorizationOpened = false;
    }
    Tools::debugLog("@@@@@ AppManager::onPopupClosed " + QString::number(openedPopupCount));
}

void AppManager::onPopupOpened()
{
    openedPopupCount++;
    Tools::debugLog("@@@@@ AppManager::onPopupOpened " + QString::number(openedPopupCount));
}

void AppManager::onDBRequestResult(const DBSelector selector, const DBRecordList& records, const bool ok)
{
    // Получен результ из БД

    Tools::debugLog("@@@@@ AppManager::onDBRequestResult " + QString::number(selector));
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
        Tools::debugLog("@@@@@ AppManager::onDBRequestResult GetShowcaseResources"  + QString::number(records.count()));
        QStringList fileNames;
        for (int i = 0; i < records.count(); i++)
        {
            QString s = getImageFileWithQmlPath(records[i]);
            // Tools::debugLog("@@@@@ AppManager::onDBRequestResult GetShowcaseResources" << s;
            fileNames.append(s);
        }
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DBSelector_GetImageByResourceCode: // Отображение картинки выбранного товара:
    {
        QString imagePath = records.count() > 0 ? getImageFileWithQmlPath(records[0]) : DUMMY_IMAGE_FILE;
        emit showProductImage(imagePath);
        Tools::debugLog("@@@@@ AppManager::onDBRequestResult showProductImage " + imagePath);
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
        if(Tools::isFileExistInDownloadPath(localFilePath))
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
    Tools::debugLog("@@@@@ AppManager::onVirtualKeyboardSet " + QString::number(v));
    onUserAction();
    emit showVirtualKeyboard(v);
}

void AppManager::onWeightPanelClicked(const int param)
{
    Tools::debugLog("@@@@@ AppManager::onWeightPanelClicked " + QString::number(param));
    if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, &AppManager::onUserAction);
    if(param == secret + 1 && ++secret == 3) onLockClicked();
}

void AppManager::onTareClicked()
{
    Tools::debugLog("@@@@@ AppManager::onTareClicked ");
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Tare);
    updateStatus();
}

void AppManager::onZeroClicked()
{
    Tools::debugLog("@@@@@ AppManager::onZeroClicked ");
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Zero);
    updateStatus();
}

void AppManager::onConfirmationClicked(const int selector)
{
    Tools::debugLog("@@@@@ AppManager::onConfirmationClicked " + QString::number(selector));
    onUserAction();
    switch (selector)
    {
    case DialogSelector::DialogSelector_Authorization:
        startAuthorization();
        break;
    case DialogSelector::DialogSelector_ClearLog:
        emit closeLogView();
        db->clearLog();
        break;
    }
}

void AppManager::onInfoClicked()
{
    Tools::debugLog("@@@@@ AppManager::onInfoClicked ");
    onUserAction();
    showMessage("Инфо", appInfo.all(EOL));
}

void AppManager::onTableResultClicked(const int index)
{
    Tools::debugLog("@@@@@ AppManager::onTableResultClicked " + QString::number(index));
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
    DBRecord* r = settings.getItemByIndexInCurrentGroup(index);
    if(r == nullptr || r->empty())
    {
        Tools::debugLog("@@@@@ AppManager::onSettingsItemClicked ERROR " + QString::number(index));
        return;
    }

    const int code = settings.getItemCode(*r);
    const QString& name = settings.getItemName(*r);
    const int type = settings.getItemType(*r);
    Tools::debugLog(QString("@@@@@ AppManager::onSettingsItemClicked %1 %2 %3").arg(QString::number(code), name, QString::number(type)));

    switch (type)
    {
    case SettingType_Group:
        settings.currentGroupCode = code;
        settingsPanelModel->update(settings);
        emit showSettingsPanel(settings.getCurrentGroupName());
        break;
    case SettingType_ReadOnly:
        showMessage(name, "Редактирование запрещено");
        break;
    case SettingType_InputNumber:
    case SettingType_InputText:
        emit showSettingInputBox(code, name, settings.getItemStringValue(*r));
        break;
    case SettingType_List:
        settingItemListModel->update(settings.getItemValueList(*r));
        emit showSettingComboBox(code, name, settings.getItemIntValue(*r), settings.getItemStringValue(*r));
        break;
    case SettingType_IntervalNumber:
    {
        QStringList list = settings.getItemValueList(*r);
        if(list.count() >= 2)
        {
            int from = Tools::stringToInt(list[0]);
            int to = Tools::stringToInt(list[1]);
            int value = settings.getItemIntValue(*r);
            emit showSettingSlider(code, name, from, to, 1, value);
        }
        break;
    }
    case SettingType_Custom:
        onCustomSettingsItemClicked(*r);
        break;
    default:
        break;
    }
}

void AppManager::clearLog()
{
    Tools::debugLog("@@@@@ AppManager::clearLog");
    showConfirmation(DialogSelector::DialogSelector_ClearLog, "Подтверждение", "Вы хотите очистить лог?");
}

void AppManager::onCustomSettingsItemClicked(const DBRecord& r)
{
    const int code = settings.getItemCode(r);
    const QString& name = settings.getItemName(r);
    Tools::debugLog(QString("@@@@@ AppManager::onCustomSettingsItemClicked %1 %2").arg(QString::number(code), name));

    if (code == SettingCode_Equipment) stopEquipment();

    switch (code)
    {
    case SettingCode_ClearLog:
        clearLog();
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
    if (code == SettingCode_Equipment) startEquipment();
}

void AppManager::onSettingsPanelCloseClicked()
{
    Tools::debugLog("@@@@@ AppManager::onSettingsPanelCloseClicked " + QString::number(settings.currentGroupCode));
    onUserAction();
    emit closeSettings();
    if(settings.currentGroupCode != 0)
    {
        DBRecord* r = settings.getItemByCode(settings.currentGroupCode);
        if(r != nullptr && !r->empty() && settings.isGroupItem(*r))
        {
            settings.currentGroupCode = r->at(SettingDBTable::GroupCode).toInt();
            settingsPanelModel->update(settings);
            emit showSettingsPanel(settings.getCurrentGroupName());
        }
    }
}

void AppManager::onSearchResultClicked(const int index)
{
    Tools::debugLog("@@@@@ AppManager::onSearchResultClicked " + QString::number(index));
    onUserAction();
    setProduct(searchPanelModel->productByIndex(index));
}

void AppManager::onShowcaseClicked(const int index)
{
    Tools::debugLog("@@@@@ AppManager::onShowcaseClicked " + QString::number(index));
    onUserAction();
    setProduct(showcasePanelModel->productByIndex(index));
}

void AppManager::onShowcaseSortClicked(const int sort)
{
    Tools::debugLog("@@@@@ AppManager::onShowcaseCodeClicked ");
    onUserAction();
    setShowcaseSort(sort);
}

void AppManager::onSwipeMainPage(const int page)
{
    Tools::debugLog("@@@@@ AppManager::onSwipeMainPage " + QString::number(page));
    emit showMainPage(page);
}

void AppManager::updateTablePanel(const bool root)
{
    Tools::debugLog("@@@@@ AppManager::updateTablePanel");
    if (root) tablePanelModel->root();
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    db->select(DBSelector_GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::startAuthorization()
{
    Tools::debugLog("@@@@@ AppManager::startAuthorization");
    isAuthorizationOpened = true;
    stopEquipment();
    const QString info = appInfo.all();
    Tools::debugLog("@@@@@ AppManager::startAuthorization " + info);
    emit showAuthorizationPanel(info);
    db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
    db->select(DBSelector_GetAuthorizationUsers, "");
    Tools::debugLog("@@@@@ AppManager::startAuthorization Done");
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    onUserAction();
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    Tools::debugLog(QString("@@@@@ AppManager::onCheckAuthorizationClick %1 %2").arg(normalizedLogin, password));
    user[UserDBTable::Name] = normalizedLogin;
    user[UserDBTable::Password] = password;
    db->select(DBSelector_GetAuthorizationUserByName, normalizedLogin);
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
        Tools::debugLog(QString("@@@@@ AppManager::stopAuthorization %1").arg(login));
    }
    else
    {
        login = user[UserDBTable::Name].toString();
        const QString password = user[UserDBTable::Password].toString();
        Tools::debugLog(QString("@@@@@ AppManager::stopAuthorization %1 %2").arg(login, password));
        if (!dbUsers.isEmpty())
        {
            if (login != dbUsers[0][UserDBTable::Name].toString() || password != dbUsers[0][UserDBTable::Password])
            {
                Tools::debugLog("@@@@@ AppManager::stopAuthorization ERROR");
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

    Tools::debugLog("@@@@@ AppManager::stopAuthorization OK");
    QString s = QString("%1. Пользователь: %2. Код: %3").arg(title, login, user[UserDBTable::Code].toString());
    db->saveLog(LogType_Warning, LogSource_User, s);
    emit showAuthorizationSucceded();

    startEquipment();
    refreshAll();
    resetProduct();
    mainPageIndex = 0;
    emit showMainPage(mainPageIndex);
    isAuthorizationOpened = false;
    if(SHOW_PATH_MESSAGE) showMessage("БД", Tools::dataBaseFilePath(DB_PRODUCT_NAME));
    onUserAction();
    Tools::debugLog("@@@@@ AppManager::stopAuthorization Done");
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

    Tools::debugLog("@@@@@ AppManager::refreshAll");
    emit showAdminMenu(UserDBTable::isAdmin(user));
    setShowcaseSort(showcaseSort);
    searchFilterModel->update();
    //searchPanelModel->update();
    filteredSearch();
    updateTablePanel(true);
}

void AppManager::showToast(const QString &title, const QString &text, const int delaySec)
{
    Tools::debugLog(QString("@@@@@ AppManager::showToast %1 %2").arg(title, text));
    emit showMessageBox(title, text, false);
    QTimer::singleShot(delaySec * 1000, this, &AppManager::hideToast);
}

void AppManager::showMessage(const QString &title, const QString &text)
{
    Tools::debugLog(QString("@@@@@ AppManager::showMessage %1 %2").arg(title, text));
    emit showMessageBox(title, text, true);
}

void AppManager::showConfirmation(const DialogSelector selector, const QString &title, const QString &text)
{
    Tools::debugLog(QString("@@@@@ AppManager::showConfirmation %1 %2 %3").arg(QString::number(selector), title, text));
    emit showConfirmationBox(selector, title, text);
}

void AppManager::resetProduct()
{
    // Сбросить выбранный продукт
    Tools::debugLog("@@@@@ AppManager::resetProduct");
    product.clear();
    printStatus.onResetProduct();
    emit resetCurrentProduct();
    updateStatus();
}

void AppManager::startEquipment(const bool server, const bool weight, const bool printer)
{
    Tools::debugLog("@@@@@ AppManager::startEquipment");
    if (server)
    {
        const int serverPort = settings.getItemIntValue(SettingCode_TCPPort);
        Tools::debugLog("@@@@@ AppManager::startEquipment serverPort = " + QString::number(serverPort));
        netServer->start(serverPort);
    }
    if (weight || printer)
    {
#ifdef Q_OS_ANDROID // --------------------------------------------------------
        QString message = "";
        QList<QString> uris = settings.parseEquipmentConfig(ANDROID_EQUIPMENT_CONFIG_FILE);
        if(uris.size() < 2) uris = settings.parseEquipmentConfig(ANDROID_DEFAULT_EQUIPMENT_CONFIG_FILE);
        if(uris.size() < 2)
        {
            uris.clear();
            uris.append("");
            uris.append("");
        }
        if(WM_DEMO || uris[0].isEmpty())
        {
            uris[0] = WEIGHT_DEMO_URI;
            message += "\nДемо-режим весового модуля";
        }
        if(PRINTER_DEMO || uris[1].isEmpty())
        {
            uris[1] = PRINTER_DEMO_URI;
            message += "\nДемо-режим принтера";
        }
#else
        QString message = "\nДемо-режим весового модуля\nДемо-режим принтера";
        QList<QString> uris;
        uris.append(WEIGHT_DEMO_URI);
        uris.append(PRINTER_DEMO_URI);
#endif // Q_OS_ANDROID --------------------------------------------------------
        Tools::debugLog("@@@@@ AppManager::startEquipment uris "+ uris.join(", "));

        int e1 = 0, e2 = 0;
        if (weight) e1 = weightManager->start(uris[0]);
        if (printer) e2 = printManager->start(uris[1]);

        if(e1 != 0) showMessage("ВНИМАНИЕ!", QString("Ошибка весового модуля %1!\n%2").
                                arg(QString::number(e1), weightManager->getErrorDescription(e1)));
        if(e2 != 0) showMessage("ВНИМАНИЕ!", QString("Ошибка принтера %1!\n%2").
                                arg(QString::number(e2), printManager->getErrorDescription(e2)));
        if(!message.isEmpty()) showMessage("ВНИМАНИЕ!", message);
    }
    Tools::debugLog("@@@@@ AppManager::startEquipment Done");
}

void AppManager::stopEquipment(const bool server, const bool weight, const bool printer)
{
    Tools::debugLog("@@@@@ AppManager::stopEquipment");
    if (server) netServer->stop();
    if (weight) weightManager->stop();
    if (printer) printManager->stop();
    Tools::debugLog("@@@@@ AppManager::stopEquipment Done");
}

void AppManager::onUserAction()
{
    Tools::debugLog("@@@@@ AppManager::onUserAction");
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
    Tools::debugLog("@@@@@ AppManager::print ");
    printManager->print(user, product, quantityAsString(product), priceAsString(product), amountAsString(product));
}

void AppManager::onPrintClicked()
{
    Tools::debugLog("@@@@@ AppManager::onPrintClicked ");
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

    Tools::debugLog("@@@@@ AppManager::onPrinted");
    db->saveLog(LogType_Error, LogSource_Print, QString("Печать. Код товара: %1. Вес/Количество: %2").arg(
                newTransaction[TransactionDBTable::ItemCode].toString(),
                newTransaction[TransactionDBTable::Weight].toString()));
    db->saveTransaction(newTransaction);
    if (settings.getItemIntValue(SettingCode_ProductReset) ==  ProductReset_Print)
        resetProduct();
}

void AppManager::onEquipmentParamChanged(const int param, const int errorCode)
{
    // Изменился параметр оборудования
    Tools::debugLog(QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(param, errorCode));

    switch (param)
    {
    case EquipmentParam_None:
        return;
    case EquipmentParam_WeightError:
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
    updateStatus();
}

void AppManager::updateStatus()
{
    const bool isProduct = !product.empty();
    const bool isPieceProduct = isProduct && ProductDBTable::isPiece(product);
    const int oldPieces = printStatus.pieces;
    if(isPieceProduct && printStatus.pieces < 1) printStatus.pieces = 1;

    const bool isZero = weightManager->isZeroFlag();
    const bool isTare = weightManager->isTareFlag();
    const bool isWeightError = weightManager->isError();
    const bool isPrintError = printManager->isError();
    const bool isFixed = weightManager->isWeightFixed();

    const bool isAutoPrint = settings.getItemBoolValue(SettingCode_PrintAuto) &&
           (!isPieceProduct || settings.getItemBoolValue(SettingCode_PrintAutoPcs));
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
    if(isProduct)
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
    const QString price = isProduct ? priceAsString(product) : "-----";
    emit showWeightParam(EquipmentParam_PriceValue, price);
    emit showWeightParam(EquipmentParam_PriceColor, isProduct ? activeColor : passiveColor);

    // Рисуем стоимость:
    const bool isAmount = isPieceProduct || (isProduct && isFixed && !isWeightError);
    const QString amount = amountAsString(product);
    emit showWeightParam(EquipmentParam_AmountValue, amount);
    emit showWeightParam(EquipmentParam_AmountColor, isAmount ? activeColor : passiveColor);

    // Можно печатать?
    if(printManager->isDemoMode()) emit showPrinterMessage("Демо режим принтера");
    printStatus.manualPrintEnabled = isAmount && !isPrintError;
    emit enableManualPrint(printStatus.manualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && printStatus.manualPrintEnabled;

    if(printStatus.calculateMode && isProduct)
    {
        if(isWeightError || isFixed || weightManager->isDemoMode()) printStatus.calculateMode = false;

        if(!isPieceProduct) // Весовой товар
        {
            if(isFixed && !isWeightError && isAutoPrintEnabled) // Автопечать
            {
                const int wg = (int)(weightManager->getWeight() * 1000); // Вес в граммах
                if(wg > 0)
                {
                    if(wg >= settings.getItemIntValue(SettingCode_PrintAutoWeight)) print();
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
    if(isPieceProduct && oldPieces != printStatus.pieces) updateStatus();

    Tools::debugLog(QString("@@@@@ AppManager::updateStatus %1 %2 %3 %4 %5 %6 %7").arg(
        Tools::boolToString(isWeightError),
        Tools::boolToString(isAutoPrint),
        Tools::boolToString(isTare),
        Tools::boolToString(isZero),
        quantity, price, amount));
}

void AppManager::beepSound()
{
    Tools::sound("qrc:/Sound/KeypressInvalid.mp3", settings.getItemIntValue(SettingCode_SoundVolume));
}

void AppManager::clickSound()
{
    Tools::sound("qrc:/Sound/KeypressStandard.mp3", settings.getItemIntValue(SettingCode_SoundVolume));
}












