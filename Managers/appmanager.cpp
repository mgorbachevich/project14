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
#include "equipmentmanager.h"
#include "netserver.h"
#include "screenmanager.h"
#include "keyemitter.h"
#include "edituserspanelmodel.h"
#include "moneycalculator.h"

AppManager::AppManager(QQmlContext* qmlContext, const QSize& screenSize, QApplication *application):
    QObject(application), context(qmlContext)
{
    Tools::removeDebugLog();
    debugLog(QString("@@@@@ AppManager::AppManager %1").arg(APP_VERSION));

    ScreenManager* screenManager = new ScreenManager(screenSize);
    context->setContextProperty("screenManager", screenManager);

    KeyEmitter* keyEmitter = new KeyEmitter(this);
    context->setContextProperty("keyEmitter", keyEmitter);

    if(CREATE_DEFAULT_DATA_ON_START &&
        !Tools::isFileExists(DB_PRODUCT_NAME))
            createDefaultData();
    if(CREATE_DEFAULT_IMAGES_ON_START &&
        !Tools::isFileExists(Tools::dbPath(QString("%1/pictures/%2").arg(DOWNLOAD_SUBDIR, "1.png"))))
            createDefaultImages();

    settings = new Settings(this);
    users = new Users(this);
    db = new DataBase(this);
    netServer = new NetServer(this);
    equipmentManager = new EquipmentManager(this);
    moneyCalculator = new MoneyCalculator(this);
    timer = new QTimer(this);

    connect(this, &AppManager::start, db, &DataBase::onAppStart);
    connect(netServer, &NetServer::action, this, &AppManager::onNetAction);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(equipmentManager, &EquipmentManager::printed, this, &AppManager::onPrinted);
    connect(equipmentManager, &EquipmentManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
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
    editUsersPanelModel = new EditUsersPanelModel(this);

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
    context->setContextProperty("editUsersPanelModel", editUsersPanelModel);

    if(!Tools::checkPermission("android.permission.READ_EXTERNAL_STORAGE") ||
       !Tools::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
        showAttention("Нет разрешений для чтения и записи конфиг.файлов");
    else if(Tools::makeDirs(false, SETTINGS_FILE).isEmpty() ||
            Tools::makeDirs(false, USERS_FILE).isEmpty())
        showAttention("Не созданы папки конфиг.файлов");

    if(REMOVE_SETTINGS_FILE_ON_START) Tools::removeFile(SETTINGS_FILE);
    settings->read();
    updateSettings(0);
    equipmentManager->create();

    // Versions:
    appInfo.appVersion = APP_VERSION;
    appInfo.dbVersion = db->version();
    appInfo.weightManagerVersion = equipmentManager->WMversion();
    appInfo.printManagerVersion = equipmentManager->PMversion();
    appInfo.netServerVersion = netServer->version();
    appInfo.ip = Tools::getNetParams().localHostIP;

    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { emit start(); });
    onUserAction();
    debugLog("@@@@@ AppManager::AppManager Done");
}

void AppManager::onDBStarted()
{
    //showToast("", "Инициализация");
    debugLog("@@@@@ AppManager::onDBStarted");
    onUserAction();
    if(db->isStarted())
    {
        startAuthorization();
        timer->start(APP_TIMER_MSEC);
    }
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
        if (isProduct() && settings->getIntValue(SettingCode_ProductReset, true) == ProductReset_Time)
        {
            quint64 waitReset = settings->getIntValue(SettingCode_ProductResetTime); // секунды
            if(waitReset > 0 && waitReset * 1000 < now - userActionTime)
            {
                debugLog("@@@@@ AppManager::onTimer reset product");
                resetProduct();
            }
        }

        // Блокировка:
        quint64 waitBlocking = settings->getIntValue(SettingCode_Blocking); // минуты
        if(waitBlocking > 0 && waitBlocking * 1000 * 60 < now - userActionTime)
        {
            debugLog("@@@@@ AppManager::onTimer blocking");
            userActionTime = now;
            startAuthorization();
        }
        else updateWeightStatus();
    }

    // Ожидание окончания сетевых запросов:
    if(netServer->isStarted() &&
       netActionTime > 0 &&
       netRoutes == 0 &&
       (WAIT_NET_ACTION_SEC * 1000) < now - netActionTime)
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
            showAttention("Данные обновлены!");
            // if(!product.isEmpty()) db->selectByParam(DataBase::RefreshCurrentProduct, product.at(ProductDBTable::Code).toString());
        }
    }
}

void AppManager::createDefaultData()
{
    debugLog("@@@@@ AppManager::createDefaultData");
    Tools::removeFile(Tools::dbPath(DB_PRODUCT_NAME));
    Tools::removeFile(Tools::dbPath(DB_LOG_NAME));
    Tools::removeFile(Tools::dbPath(DB_TEMP_NAME));
    Tools::removeFile(Tools::dbPath(DEBUG_LOG_NAME));
    Tools::copyFile(QString(":/Default/%1").arg(DB_PRODUCT_NAME), Tools::dbPath(DB_PRODUCT_NAME));
}

void AppManager::createDefaultImages()
{
    debugLog("@@@@@ AppManager::createDefaultImages");
    QStringList images = { "1.png", "2.png", "3.jpg", "4.png", "5.png", "6.png", "8.png",
                           "9.png", "10.png", "11.png", "12.png", "15.png" };
    for (int i = 0; i < images.count(); i++)
        Tools::copyFile(QString(":/Default/%1").arg(images[i]),
                        Tools::dbPath(QString("%1/pictures/%2").arg(DOWNLOAD_SUBDIR, images[i])));
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
    if(ProductDBTable::isPiece(product))
    {
        debugLog(QString("@@@@@ AppManager::onProductPanelPiecesClicked %1 %2").arg(
                     Tools::intToString(printStatus.pieces),
                     Tools::intToString(settings->getIntValue(SettingCode_CharNumberPieces))));
        emit showPiecesInputBox(printStatus.pieces, settings->getIntValue(SettingCode_CharNumberPieces));
    }
    else beepSound();
}

void AppManager::onRewind() // Перемотка
{
    debugLog("@@@@@ AppManager::onRewind ");
    if(isAuthorizationOpened() || isSettingsOpened()) beepSound();
    else equipmentManager->feed();
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
    DBRecord* r = settings->getByCode(settingItemCode);
    if (r == nullptr)
    {
        showAttention("Ошибка настройки (неизвестная запись)!");
        return;
    }
    if(!settings->onInputValue(settingItemCode, value)) return;
    updateSettings(settings->getCurrentGroupCode());
    settings->write();
    QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(QString::number(settingItemCode), value);
    db->saveLog(LogType_Warning, LogSource_Admin, s);
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
    const int maxChars = settings->getIntValue(SettingCode_CharNumberPieces);
    if(value.length() > maxChars)
    {
        v = Tools::stringToInt(value.leftJustified(maxChars));
        showAttention("Максимальная длина " + Tools::intToString(maxChars));
    }
    if(v < 1)
    {
        v = 1;
        showAttention("Количество не должно быть меньше 1");
    }
    printStatus.pieces = v;
    updateWeightStatus();
}

void AppManager::onSetProductByCodeClicked(const QString &value)
{
    debugLog("@@@@@ AppManager::onSetProductByCodeClicked " + value);
    db->select(DBSelector_SetProductByInputCode, value.split(PRODUCT_STRING_DELIMETER).at(0));
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
        showAttention("Ошибка базы данных!");
        return;
    }

    switch(selector)
    {
    case DBSelector_GetShowcaseProducts: // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        db->select(DBSelector_GetShowcaseResources, records);
        break;

    case DBSelector_GetMessageByResourceCode: // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            showMessage("Описание товара", records[0][ResourceDBTable::Value].toString());
        break;

    case DBSelector_GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DBSelector_GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        tablePanelModel->update(records);
        break;

    case DBSelector_SetProductByInputCode: // Отображение товара с заданным кодом:
        if(records.isEmpty())
            showAttention("Товар не найден!");
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
    if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, [this]() { onUserAction(); } );
    if(param == secret + 1 && (++secret) == 3) onLockClicked();
}

void AppManager::onTareClicked()
{
    debugLog("@@@@@ AppManager::onTareClicked ");
    onUserAction();
    equipmentManager->setWMParam(ControlParam_Tare);
    updateWeightStatus();
}

void AppManager::onZeroClicked()
{
    debugLog("@@@@@ AppManager::onZeroClicked ");
    onUserAction();
    equipmentManager->setWMParam(ControlParam_Zero);
    updateWeightStatus();
}

void AppManager::onConfirmationClicked(const int selector)
{
    debugLog("@@@@@ AppManager::onConfirmationClicked " + Tools::intToString(selector));
    onUserAction();
    switch (selector)
    {
    case ConfirmSelector_DeleteUser:
        users->deleteInputUser();
        editUsersPanelModel->update(users);
        break;
    case ConfirmSelector_ReplaceUser:
        users->replaceOrInsertInputUser();
        editUsersPanelModel->update(users);
        break;
    case ConfirmSelector_Authorization:
        startAuthorization();
        break;
    case ConfirmSelector_ClearLog:
        emit closeLogView();
        db->clearLog();
        break;
    case ConfirmSelector_SetSystemDateTime:
        equipmentManager->setSystemDateTime(true);
        break;
    }
}

void AppManager::onDeleteUserClicked(const QString& code)
{
    debugLog("@@@@@ AppManager::onDeleteUserClicked " + code);
    users->onDeleteUser(code);
}

void AppManager::onInfoClicked()
{
    debugLog("@@@@@ AppManager::onInfoClicked ");
    onUserAction();
    showMessage("Инфо", appInfo.all("\r\n"));
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
    DBRecord* r = settings->getByIndexInCurrentGroup(index);
    if(r == nullptr || r->empty())
    {
        debugLog("@@@@@ AppManager::onSettingsItemClicked ERROR " + Tools::intToString(index));
        return;
    }

    const int code = settings->getCode(*r);
    const QString& name = settings->getName(*r);
    const int type = settings->getType(*r);
    debugLog(QString("@@@@@ AppManager::onSettingsItemClicked %1 %2 %3").arg(Tools::intToString(code), name, QString::number(type)));

    switch (type)
    {
    case SettingType_Group:
        updateSettings(code);
        emit showSettingsPanel(settings->getCurrentGroupName());
        break;
    case SettingType_InputNumber:
    case SettingType_InputText:
        emit showSettingInputBox(code, name, settings->getStringValue(*r));
        break;
    case SettingType_List:
        settingItemListModel->update(settings->getValueList(*r));
        emit showSettingComboBox(code, name, settings->getIntValue(*r, true), settings->getStringValue(*r), settings->getComment(*r));
        break;
    case SettingType_IntervalNumber:
    {
        QStringList list = settings->getValueList(*r);
        if(list.count() >= 2)
        {
            int from = Tools::stringToInt(list[0]);
            int to = Tools::stringToInt(list[1]);
            int value = settings->getIntValue(*r);
            emit showSettingSlider(code, name, from, to, 1, value);
        }
        break;
    }
    case SettingType_Custom:
        onCustomSettingsItemClicked(*r);
        break;
        break;
    case SettingType_Unsed:
        showMessage(name, "Не поддерживается");
        break;
    case SettingType_ReadOnly:
        showMessage(name, "Редактирование запрещено");
        break;
    case SettingType_GroupWithPassword:
        emit showPasswordInputBox(code);
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
    const int code = settings->getCode(r);
    const QString& name = settings->getName(r);
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
    case SettingCode_Ethernet:
        settings->nativeSettings(code);
        break;
    case SettingCode_SystemSettings:
        emit showPasswordInputBox(code);
        break;
    default:
        showMessage(name, "Не поддерживается");
        return;
    }
}

void AppManager::updateSettings(const int groupCode)
{
    settings->update(groupCode);
    settingsPanelModel->update(*settings);
}

void AppManager::onSettingsPanelCloseClicked()
{
    debugLog("@@@@@ AppManager::onSettingsPanelCloseClicked " + Tools::intToString(settings->getCurrentGroupCode()));
    onUserAction();
    emit previousSettings();
    const int groupCode = settings->getCurrentGroupCode();
    if(groupCode != 0)
    {
        DBRecord* r = settings->getByCode(groupCode);
        if(r != nullptr && !r->empty() && settings->isGroup(*r))
        {
            // Переход вверх:
            updateSettings(r->at(SettingField_GroupCode).toInt());
            emit showSettingsPanel(settings->getCurrentGroupName());
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
    QString normalizedLogin = Users::fromAdminName(login);
    debugLog(QString("@@@@@ AppManager::onCheckAuthorizationClick %1 %2").arg(normalizedLogin, password));
    stopAuthorization(normalizedLogin, password);
}

void AppManager::startSettings()
{
    debugLog("@@@@@ AppManager::startSettings");
    isSettings = true;
    resetProduct();
    stopEquipment();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    emit showSettingsPanel(settings->getCurrentGroupName());
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
    resetProduct();
    stopEquipment();
    setMainPage(-1);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::startAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        updateSystemStatus();
        db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
        showUsers();
        debugLog("@@@@@ AppManager::startAuthorization Done");
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
    emit showAdminMenu(users->isAdmin(users->getUser()));
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
    QTimer::singleShot(delaySec * 1000, this, [this]() { emit hideToast(); } );
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
        productPanelModel->update(product, moneyCalculator->price(product), (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
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

void AppManager::onUserAction()
{
    debugLog("@@@@@ AppManager::onUserAction");
    if(DEBUG_MEMORY_MESSAGE) Tools::debugMemory();
    userActionTime = Tools::currentDateTimeToUInt();
    secret = 0;
}

void AppManager::print() // Печатаем этикетку
{
    debugLog("@@@@@ AppManager::print ");
    equipmentManager->print(users->getUser(),
                            product,
                            moneyCalculator->quantityAsString(product),
                            moneyCalculator->priceAsString(product),
                            moneyCalculator->amountAsString(product));
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
    if (settings->getIntValue(SettingCode_ProductReset, true) == ProductReset_Print) isResetProductNeeded = true;
}

void AppManager::onEquipmentParamChanged(const int param, const int errorCode)
{
    // Изменился параметр оборудования
    if(DEBUG_WEIGHT_STATUS) debugLog(QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(
                 Tools::intToString(param), Tools::intToString(errorCode)));

    switch (param)
    {
    case ControlParam_None:
        return;
    case ControlParam_WeightValue:
        if(isResetProductNeeded) resetProduct();
        break;
    case ControlParam_WeightError:
        if(isResetProductNeeded) resetProduct();
        db->saveLog(LogType_Error, LogSource_Weight, QString("Ошибка весового модуля. Код: %1. Описание: %2").arg(
                    QString::number(errorCode),
                    equipmentManager->getWMErrorDescription(errorCode)));
        break;
    case ControlParam_PrintError:
    {
        db->saveLog(LogType_Error, LogSource_Print, QString("Ошибка принтера. Код: %1. Описание: %2").arg(
                    QString::number(errorCode),
                    equipmentManager->getPMErrorDescription(errorCode)));
        emit showWeightParam(ControlParam_PrinterStatus, errorCode == 0 ? "" :
                    equipmentManager->getPMErrorDescription(errorCode));
        break;
    }
    }
    updateWeightStatus();
}

void AppManager::updateWeightStatus()
{
    if(DEBUG_WEIGHT_STATUS) debugLog("@@@@@ AppManager::updateWeightStatus");

    const bool isPieceProduct = isProduct() && ProductDBTable::isPiece(product);
    const int oldPieces = printStatus.pieces;
    if(isPieceProduct && printStatus.pieces < 1) printStatus.pieces = 1;

    const bool isWM = equipmentManager->isWM();
    const bool isPM = equipmentManager->isPM();
    const bool isZero = equipmentManager->isZeroFlag();
    const bool isTare = equipmentManager->isTareFlag();
    const bool isWMError = equipmentManager->isWMError() || !isWM;
    const bool isFixed = equipmentManager->isWeightFixed();

    const bool isAutoPrint = settings->getBoolValue(SettingCode_PrintAuto) &&
           (!isPieceProduct || settings->getBoolValue(SettingCode_PrintAutoPcs));
    const QString passiveColor = "#424242";
    const QString activeColor = "#fafafa";

    // Проверка флага 0 - новый товар на весах (начинать обязательно с этого!):
    if(isZero) printStatus.calculateMode = true;

    // Рисуем флажки:
    emit showWeightParam(ControlParam_Zero, Tools::boolToString(isZero));
    emit showWeightParam(ControlParam_Tare, Tools::boolToString(isTare));
    emit showWeightParam(ControlParam_WeightFixed, Tools::boolToString(isFixed));
    if(isWMError)
        emit showWeightParam(ControlParam_WeightError, Tools::boolToString(true));
    else if(isAutoPrint)
        emit showWeightParam(ControlParam_AutoPrint, Tools::boolToString(true));
    else
        emit showWeightParam(ControlParam_WeightError, Tools::boolToString(false));

    // Рисуем загаловки:
    QString wt = isPieceProduct ? "КОЛИЧЕСТВО, шт" : "МАССА, кг";
    if(equipmentManager->isWMDemoMode()) wt += " ДЕМО";
    QString pt = "ЦЕНА, руб";
    if(isProduct())
    {
        if (isPieceProduct) pt += "/шт";
        else pt += ProductDBTable::is100gBase(product) ? "/100г" : "/кг";
    }
    emit showWeightParam(ControlParam_WeightTitle, wt);
    emit showWeightParam(ControlParam_PriceTitle, pt);
    emit showWeightParam(ControlParam_AmountTitle, "СТОИМОСТЬ, руб");

    // Рисуем количество (вес/штуки):
    QString quantity = NO_DATA;
    if(isWM) quantity = isPieceProduct || !isWMError ? moneyCalculator->quantityAsString(product) : "";
    emit showWeightParam(ControlParam_WeightValue, quantity);
    emit showWeightParam(ControlParam_WeightColor, isPieceProduct || (isFixed && !isWMError) ? activeColor : passiveColor);

    // Рисуем цену:
    QString ps = moneyCalculator->priceAsString(product);
    bool isPrice = isProduct() && PRICE_MAX_CHARS >= ps.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString price = isPrice ? moneyCalculator->priceAsString(product) : NO_DATA;
    emit showWeightParam(ControlParam_PriceValue, price);
    emit showWeightParam(ControlParam_PriceColor, isPrice ? activeColor : passiveColor);

    // Рисуем стоимость:
    QString as = moneyCalculator->amountAsString(product);
    bool isAmount = isWM && isPrice && (isPieceProduct || (isFixed && !isWMError)) &&
        AMOUNT_MAX_CHARS >= as.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString amount = isAmount ? moneyCalculator->amountAsString(product) : NO_DATA;
    emit showWeightParam(ControlParam_AmountValue, amount);
    emit showWeightParam(ControlParam_AmountColor, isAmount ? activeColor : passiveColor);

    // Печатать?
    printStatus.manualPrintEnabled = isAmount && isPM && !equipmentManager->isPMError();
    emit enableManualPrint(printStatus.manualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && printStatus.manualPrintEnabled;
    emit showWeightParam(ControlParam_PrinterStatus, isWM && equipmentManager->isPMDemoMode() ? "<b>ДЕМО</b>" : "");

    // Автопечать
    if(printStatus.calculateMode && isPrice)
    {
        if(isWMError || isFixed) printStatus.calculateMode = false;
        if(!isPieceProduct) // Весовой товар
        {
            const int wg = (int)(equipmentManager->getWeight() * 1000); // Вес в граммах
            if(!isWMError && isFixed && isAutoPrintEnabled && wg > 0)
            {
                if(wg >= settings->getIntValue(SettingCode_PrintAutoWeight)) print(); // Автопечать
                else showToast("ВНИМАНИЕ!", "Вес слишком мал для автопечати");
            }
        }
        else // Штучный товар
        {
            const double unitWeight = product[ProductDBTable::UnitWeight].toDouble() / 1000000; // Вес единицы, кг
            if(unitWeight > 0) // Задан вес единицы
            {
                if(isFixed && !isWMError)
                {
                    int newPieces = (int)(equipmentManager->getWeight() / unitWeight + 0.5); // Округление до ближайшего целого
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
    if(isPieceProduct && oldPieces != printStatus.pieces)
    {
        updateWeightStatus();
        return;
    }

    if(DEBUG_WEIGHT_STATUS)
        debugLog(QString("@@@@@ AppManager::updateWeightStatus %1%2%3%4%5 %6%7%8%9%10 %11 %12 %13").arg(
                 Tools::boolToIntString(isWMError),
                 Tools::boolToIntString(isAutoPrint),
                 Tools::boolToIntString(isFixed),
                 Tools::boolToIntString(isTare),
                 Tools::boolToIntString(isZero),
                 Tools::boolToIntString(isWM),
                 Tools::boolToIntString(isPM),
                 Tools::boolToIntString(isPieceProduct),
                 Tools::boolToIntString(printStatus.manualPrintEnabled),
                 Tools::boolToIntString(isAutoPrintEnabled),
                 quantity, price, amount));
}

void AppManager::beepSound()
{
    Tools::sound("qrc:/Sound/KeypressInvalid.mp3", settings->getIntValue(SettingCode_KeyboardSoundVolume));
}

void AppManager::clickSound()
{
    Tools::sound("qrc:/Sound/KeypressStandard.mp3", settings->getIntValue(SettingCode_KeyboardSoundVolume));
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

void AppManager::stopEquipment()
{
    debugLog("@@@@@ AppManager::stopEquipment");
    timer->blockSignals(true);
    netServer->stop();
    equipmentManager->stop();
    debugLog("@@@@@ AppManager::stopEquipment Done");
}

void AppManager::startEquipment()
{
    debugLog("@@@@@ AppManager::startEquipment");

    const int serverPort = settings->getIntValue(SettingCode_TCPPort);
    debugLog("@@@@@ AppManager::startEquipment serverPort = " + QString::number(serverPort));
    netServer->start(serverPort);
    equipmentManager->start();

    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        timer->blockSignals(false);
        debugLog("@@@@@ AppManager::startEquipment Done");
    });
}

void AppManager::onEditUsersClicked()
{
    users->getAll();
    debugLog("@@@@@ AppManager::onEditUsersClicked " + Tools::intToString(users->count()));
    editUsersPanelModel->update(users);
    emit showEditUsersPanel();
}

void AppManager::onEditUsersPanelClicked(const int index)
{
    DBRecord u = users->get(index);
    const int code = users->getCode(u);
    if(code <= 0)
    {
        showAttention("Редактирование запрещено");
        return;
    }
    const QString name = users->getName(u);
    const QString password = users->getPassword(u);
    const bool isAdmin = users->isAdmin(u);
    debugLog(QString("@@@@@ AppManager::onEditUsersPanelClicked %1 %2 %3 %4 %5").arg(
        Tools::intToString(index), Tools::intToString(code), name, password, Tools::boolToString(isAdmin)));
    emit showInputUserPanel(Tools::intToString(code), name, password, isAdmin);
}

void AppManager::onEditUsersPanelClose()
{
    debugLog("@@@@@ AppManager::onEditUsersPanelClose ");
    users->clear();
}

void AppManager::onAddUserClicked()
{
    debugLog("@@@@@ AppManager::onAddUserClicked ");
    emit showInputUserPanel("", "", "", false);
}

void AppManager::onInputUserClosed(const QString& code, const QString& name, const QString& password, const bool admin)
{
    users->onInputUser(code, name, password, admin);
    editUsersPanelModel->update(users);
}

void AppManager::showUsers()
{
    // Обновить список пользователей на экране авторизации
    DBRecordList records = users->getAll();
    userNameModel->update(records);
    const int currentUserCode = Users::getCode(users->getUser());
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord& r = records[i];
        if(records.count() == 1 || Users::getCode(r) == currentUserCode)
        {
            emit showCurrentUser(i, Users::getName(r));
            break;
        }
    }
}

void AppManager::stopAuthorization(const QString& login, const QString& password)
{
    const QString title = "Авторизация";
    if(CHECK_AUTHORIZATION)
    {
        debugLog(QString("@@@@@ AppManager::stopAuthorization %1 %2").arg(login, password));
        DBRecord u = users->getByName(login);
        if (u.isEmpty() || password != Users::getPassword(u))
        {
            debugLog("@@@@@ AppManager::stopAuthorization ERROR");
            const QString error = "Неверные имя пользователя или пароль";
            showMessage(title, error);
            db->saveLog(LogType_Warning, LogSource_User, QString("%1. %2").arg(title, error));
            onUserAction();
            return;
        }
        users->setUser(u);
    }

    debugLog("@@@@@ AppManager::stopAuthorization OK");
    db->saveLog(LogType_Warning, LogSource_User, QString("%1. Пользователь: %2. Код: %3").arg(
                    title, login, Tools::intToString(Users::getCode(users->getUser()))));
    setMainPage(0);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::stopAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        startEquipment();
        refreshAll();
        updateWeightStatus();
        onUserAction();
        if(SHOW_PATH_MESSAGE) showMessage("БД", Tools::dbPath(DB_PRODUCT_NAME));
        users->clear();
        debugLog("@@@@@ AppManager::stopAuthorization Done");
    });
}

void AppManager::onPasswordInputClosed(const int code, const QString& inputPassword)
{
    switch(code)
    {
    case SettingCode_FactorySettings:
        if(inputPassword == settings->getScaleConfigValue(ScaleConfigField_FactorySettingsPassword))
        {
            updateSettings(code);
            emit showSettingsPanel(settings->getCurrentGroupName());
        }
        else showAttention("Неверный пароль");
        break;
    case SettingCode_SystemSettings:
        if(inputPassword == settings->getScaleConfigValue(ScaleConfigField_SystemSettingsPassword))
            settings->nativeSettings(code);
        else showAttention("Неверный пароль");
        break;
    default:
        break;
    }
}









