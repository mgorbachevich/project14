#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include <QtConcurrent/QtConcurrent>
#include "baselistmodel.h"
#include "appmanager.h"
#include "resourcedbtable.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "productpanelmodel.h"
#include "usernamemodel.h"
#include "showcasepanelmodel3.h"
#include "searchpanelmodel3.h"
#include "inputproductcodepanelmodel3.h"
#include "settingspanelmodel3.h"
#include "viewlogpanelmodel.h"
#include "searchfiltermodel.h"
#include "tools.h"
#include "appinfo.h"
#include "equipmentmanager.h"
#include "netserver.h"
#include "screenmanager.h"
#include "keyemitter.h"
#include "edituserspanelmodel3.h"

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
    connect(netServer, &NetServer::netCommand, this, &AppManager::onNetCommand);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(equipmentManager, &EquipmentManager::printed, this, &AppManager::onPrinted);
    connect(equipmentManager, &EquipmentManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
    connect(keyEmitter, &KeyEmitter::enterChar, this, &AppManager::onEnterChar);
    connect(keyEmitter, &KeyEmitter::enterKey, this, &AppManager::onEnterKey);
    connect(timer, &QTimer::timeout, this, &AppManager::onTimer);

    productPanelModel = new ProductPanelModel(this);
    showcasePanelModel = new ShowcasePanelModel3(this);
    settingsPanelModel = new SettingsPanelModel3(this);
    searchPanelModel = new SearchPanelModel3(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    viewLogPanelModel = new ViewLogPanelModel(this);
    inputProductCodePanelModel = new InputProductCodePanelModel3(this);
    editUsersPanelModel = new EditUsersPanelModel3(this);
    settingItemModel = new BaseListModel(this);
    calendarDayModel = new BaseListModel(this);
    calendarMonthModel = new BaseListModel(this);
    calendarYearModel = new BaseListModel(this);

    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("settingsPanelModel", settingsPanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);
    context->setContextProperty("viewLogPanelModel", viewLogPanelModel);
    context->setContextProperty("inputProductCodePanelModel", inputProductCodePanelModel);
    context->setContextProperty("editUsersPanelModel", editUsersPanelModel);
    context->setContextProperty("settingItemModel", settingItemModel);
    context->setContextProperty("calendarDayModel", calendarDayModel);
    context->setContextProperty("calendarMonthModel", calendarMonthModel);
    context->setContextProperty("calendarYearModel", calendarYearModel);

    QStringList days, months, years;
    for (int i = 1; i <= 31; i++) days << Tools::intToString(i);
    for (int i = 1; i <= 12; i++) months << Tools::intToString(i);
    for (int i = 1; i <= 25; i++) years << Tools::intToString(i + 2023);
    calendarDayModel->update(days);
    calendarMonthModel->update(months);
    calendarYearModel->update(years);

    if(!Tools::checkPermission("android.permission.READ_EXTERNAL_STORAGE") ||
       !Tools::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
        showAttention("Нет разрешений для чтения и записи конфиг.файлов");
    else if(Tools::makeDirs(false, SETTINGS_FILE).isEmpty() ||
            Tools::makeDirs(false, USERS_FILE).isEmpty())
        showAttention("Не созданы папки конфиг.файлов");

    if(REMOVE_SETTINGS_FILE_ON_START) Tools::removeFile(SETTINGS_FILE);
    settings->read();
    updateSettings(0);
    settings->apply();
    equipmentManager->create();

    // Versions:
    appInfo.appVersion = APP_VERSION;
    appInfo.dbVersion = db->version();
    appInfo.weightManagerVersion = equipmentManager->WMversion();
    appInfo.printManagerVersion = equipmentManager->PMversion();
    appInfo.netServerVersion = netServer->version();
    appInfo.ip = Tools::getNetParams().localHostIP;

    onUserAction();
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { emit start(); });
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
    if(netServer->isStarted() && netCommandTime > 0 && (WAIT_NET_COMMAND_SEC * 1000) < now - netCommandTime)
    {
        debugLog("@@@@@ AppManager::onTimer netCommandTime");
        onNetCommand(NetCommand_StopLoad, "");
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

void AppManager::onProductFavoriteClicked()
{
    debugLog("@@@@@ AppManager::onProductFavoriteClicked");
    onUserAction();
    if(isAdmin() || settings->getBoolValue(SettingCode_ChangeShowcase))
    {
        if(db->isInShowcase(product))
            showConfirmation(ConfirmSelector_RemoveFromShowcase,  "Удалить товар из витрины?");
        else
            showConfirmation(ConfirmSelector_AddToShowcase, "Добавить товар в витрину?");
    }
}

void AppManager::onProductPanelCloseClicked()
{
    onUserAction();
    resetProduct();
    setMainPage(mainPageIndex);
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
    if(!settings->setValue(settingItemCode, value)) return;
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
    showConfirmation(ConfirmSelector::ConfirmSelector_Authorization, "Вы хотите сменить пользователя?");
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
    QString s = value;
    db->select(DBSelector_SetProductByInputCode, s.remove("#"));
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
    case DBSelector_GetMessageByResourceCode: // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            showMessage("Описание товара", records[0][ResourceDBTable::Value].toString());
        break;

    case DBSelector_GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DBSelector_GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        emit showHierarchyRoot(searchPanelModel->isHierarchyRoot());
        emit showControlParam(ControlParam_SearchTitle, searchPanelModel->hierarchyTitle());
        searchPanelModel->update(records, -1);
        break;

    case DBSelector_SetProductByInputCode: // Отображение товара с заданным кодом:
        if(records.isEmpty()) showAttention("Товар не найден!");
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
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по коду");
        searchPanelModel->update(records, SearchFilterIndex_Code);
        break;

    case DBSelector_GetProductsByFilteredCode2: // Отображение товаров с заданным фрагментом номера:
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по номеру");
        searchPanelModel->update(records, SearchFilterIndex_Code2);
        break;

    case DBSelector_GetProductsByFilteredBarcode: // Отображение товаров с заданным фрагментом штрих-кода:
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по штрих-коду");
        searchPanelModel->update(records, SearchFilterIndex_Barcode);
        break;

    case DBSelector_GetProductsByFilteredName: // Отображение товаров с заданным фрагментом наименования:
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по наименованию");
        searchPanelModel->update(records, SearchFilterIndex_Name);
        break;

   case DBSelector_RefreshCurrentProduct: // Сброс выбранного товара после изменений в БД:
        resetProduct();
        if (!records.isEmpty() && !records.at(0).isEmpty())
        {
            //showToast("ВНИМАНИЕ!", "Выбранный товар изменен");
            setProduct(records.at(0));
        }
        break;

    case DBSelector_GetShowcaseProducts: // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        db->select(DBSelector_GetShowcaseResources, records);
        emit showShowcaseSort(showcasePanelModel->sort, showcasePanelModel->increase);
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
        emit showControlParam(ControlParam_ProductImage, imagePath);
        debugLog("@@@@@ AppManager::onDBRequestResult showProductImage " + imagePath);
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
    case ConfirmSelector_RemoveFromShowcase:
        db->removeFromShowcase(product);
        emit setCurrentProductFavorite(db->isInShowcase(product));
        updateShowcase();
        break;
    case ConfirmSelector_AddToShowcase:
        db->addToShowcase(product);
        emit setCurrentProductFavorite(db->isInShowcase(product));
        updateShowcase();
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
        settingItemModel->update(settings->getValueList(*r));
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
    case SettingType_UnsedGroup:
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
    showConfirmation(ConfirmSelector_ClearLog, "Вы хотите очистить лог?");
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
        setSystemDateTime();
        break;
    case SettingCode_Equipment:
    case SettingCode_WiFi:
    case SettingCode_Ethernet:
        settings->nativeSettings(code);
        break;
    case SettingCode_SystemSettings:
        emit showPasswordInputBox(code);
        break;
    case SettingCode_Users:
        onEditUsersClicked();
        break;
    case SettingCode_VerificationDate:
        showVerificationDateInputPanel();
        break;
    default:
        showMessage(name, "Не поддерживается");
        return;
    }
}

void AppManager::showVerificationDateInputPanel()
{
    QStringList value = settings->getStringValue(SettingCode_VerificationDate).split(".");
    int d = 1;
    int m = 1;
    int y = 2024;
    if(value.count() > 0) d = Tools::stringToInt(value[0]);
    if(value.count() > 1) m = Tools::stringToInt(value[1]);
    if(value.count() > 2) y = Tools::stringToInt(value[2]);
    QDate date(y, m, d);
    if(!date.isValid()) { d = m = 1; y = 2024; }
    emit showCalendarPanel(d, m, y);
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

void AppManager::onShowcaseClicked(const int index)
{
    debugLog("@@@@@ AppManager::onShowcaseClicked " + Tools::intToString(index));
    onUserAction();
    setProduct(showcasePanelModel->productByIndex(index));
}

void AppManager::onShowcaseDirectionClicked()
{
    onUserAction();
    showcasePanelModel->increase = !showcasePanelModel->increase;
    updateShowcase();
}

void AppManager::onShowcaseSortClicked(const int sort)
{
    debugLog("@@@@@ AppManager::onShowcaseSortClicked " + Tools::intToString(sort));
    onUserAction();
    showcasePanelModel->sort = sort;
    updateShowcase();
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

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    onUserAction();
    QString name = Users::normalizedName(login);
    debugLog(QString("@@@@@ AppManager::onCheckAuthorizationClick %1 %2").arg(name, password));
    stopAuthorization(name, password);
}

void AppManager::startSettings()
{
    debugLog("@@@@@ AppManager::startSettings");
    isSettings = true;
    stopAll();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    emit showSettingsPanel(settings->getCurrentGroupName());
}

void AppManager::stopSettings()
{
    debugLog("@@@@@ AppManager::stopSettings");
    refreshAll();
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        startAll();
        isSettings = false;
    });
}

void AppManager::startAuthorization()
{
    debugLog("@@@@@ AppManager::startAuthorization");
    stopAll();
    setMainPage(MainPageIndex_Authorization);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::startAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        updateSystemStatus();
        db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
        showAuthorizationUsers();
        emit showControlParam(ControlParam_AuthorizationTitle1, QString("%1   № %2").arg(
                                  settings->getStringValue(SettingCode_ScalesName),
                                  settings->getStringValue(SettingCode_SerialScalesNumber)));
        emit showControlParam(ControlParam_AuthorizationTitle2, equipmentManager->getWMDescription());
        emit showControlParam(ControlParam_AuthorizationTitle3, QString("Поверка %1").arg(
                                  settings->getStringValue(SettingCode_VerificationDate)));
        debugLog("@@@@@ AppManager::startAuthorization Done");
    });
}

void AppManager::updateShowcase()
{
    db->select(DBSelector_GetShowcaseProducts,
               Tools::intToString(showcasePanelModel->sort),
               Tools::boolToString(showcasePanelModel->increase));
}

void AppManager::refreshAll()
{
    // Обновить всё на экране
    debugLog("@@@@@ AppManager::refreshAll");
    resetProduct();
    setMainPage(mainPageIndex);
    updateShowcase();
    emit showVirtualKeyboard(-1);
    updateSearch("", true);
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
    emit showControlParam(ControlParam_DateTime, dateTime);
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

void AppManager::showConfirmation(const ConfirmSelector selector, const QString &text)
{
    showConfirmation(selector, "Подтверждение", text);
}

void AppManager::netDownload(QHash<DBTable *, QList<QVariantList> > rs, int &s, int &e)
{
    netCommandTime = Tools::currentDateTimeToUInt();
    db->netDownload(rs, s, e);
}

QString AppManager::netDelete(const QString &t, const QString &s)
{
    netCommandTime = Tools::currentDateTimeToUInt();
    return db->netDelete(t, s);
}

QString AppManager::netUpload(const QString &t, const QString &s, const bool b)
{
    netCommandTime = Tools::currentDateTimeToUInt();
    return db->netUpload(t, s, b);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    if(!product.isEmpty())
    {
        QString productCode = product[ProductDBTable::Code].toString();
        debugLog("@@@@@ AppManager::setProduct " + productCode);
        productPanelModel->update(product, priceAsString(product), (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
        emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
        db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
        QString pictureCode = product[ProductDBTable::PictureCode].toString();
        db->select(DBSelector_GetImageByResourceCode, pictureCode);
        printStatus.onNewProduct();
        emit setCurrentProductFavorite(db->isInShowcase(product));
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
    }
    updateWeightStatus();
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
    equipmentManager->print(db,
                            users->getCurrentUser(),
                            product,
                            moneyCalculator->quantityAsString(product),
                            priceAsString(product),
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
        emit showControlParam(ControlParam_PrinterStatus, errorCode == 0 ? "" :
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
    emit showControlParam(ControlParam_Zero, Tools::boolToString(isZero));
    emit showControlParam(ControlParam_Tare, Tools::boolToString(isTare));
    emit showControlParam(ControlParam_WeightFixed, Tools::boolToString(isFixed));
    if(isWMError)
        emit showControlParam(ControlParam_WeightError, Tools::boolToString(true));
    else if(isAutoPrint)
        emit showControlParam(ControlParam_AutoPrint, Tools::boolToString(true));
    else
        emit showControlParam(ControlParam_WeightError, Tools::boolToString(false));

    // Рисуем загаловки:
    QString wt = isPieceProduct ? "КОЛИЧЕСТВО, шт" : "МАССА, кг";
    if(equipmentManager->isWMDemoMode()) wt += " ДЕМО";
    QString pt = "ЦЕНА, руб";
    if(isProduct())
    {
        if (isPieceProduct) pt += "/шт";
        else pt += ProductDBTable::is100gBase(product) ? "/100г" : "/кг";
    }
    emit showControlParam(ControlParam_WeightTitle, wt);
    emit showControlParam(ControlParam_PriceTitle, pt);
    emit showControlParam(ControlParam_AmountTitle, "СТОИМОСТЬ, руб");

    // Рисуем количество (вес/штуки):
    QString quantity = NO_DATA;
    if(isWM) quantity = isPieceProduct || !isWMError ? moneyCalculator->quantityAsString(product) : "";
    emit showControlParam(ControlParam_WeightValue, quantity);
    emit showControlParam(ControlParam_WeightColor, isPieceProduct || (isFixed && !isWMError) ? activeColor : passiveColor);

    // Рисуем цену:
    QString ps = moneyCalculator->priceAsString(product);
    bool isPrice = isProduct() && PRICE_MAX_CHARS >= ps.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString price = isPrice ? priceAsString(product) : NO_DATA;
    emit showControlParam(ControlParam_PriceValue, price);
    emit showControlParam(ControlParam_PriceColor, isPrice ? activeColor : passiveColor);

    // Рисуем стоимость:
    QString as = moneyCalculator->amountAsString(product);
    bool isAmount = isWM && isPrice && (isPieceProduct || (isFixed && !isWMError)) &&
        AMOUNT_MAX_CHARS >= as.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString amount = isAmount ? moneyCalculator->amountAsString(product) : NO_DATA;
    emit showControlParam(ControlParam_AmountValue, amount);
    emit showControlParam(ControlParam_AmountColor, isAmount ? activeColor : passiveColor);

    // Печатать?
    printStatus.manualPrintEnabled = isAmount && isPM && !equipmentManager->isPMError();
    emit enableManualPrint(printStatus.manualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && printStatus.manualPrintEnabled;
    emit showControlParam(ControlParam_PrinterStatus, isWM && equipmentManager->isPMDemoMode() ? "<b>ДЕМО</b>" : "");

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

void AppManager::setSystemDateTime()
{
    debugLog("@@@@@ AppManager::setSystemDateTime");
    showConfirmation(ConfirmSelector_SetSystemDateTime, "Вы хотите установить системное время?");
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

void AppManager::stopAll()
{
    debugLog("@@@@@ AppManager::stopAll");
    resetProduct();
    timer->blockSignals(true);
    netServer->stop();
    equipmentManager->stop();
    debugLog("@@@@@ AppManager::stopAll Done");
}

void AppManager::startAll()
{
    debugLog("@@@@@ AppManager::startAll");

    const int serverPort = settings->getIntValue(SettingCode_TCPPort);
    debugLog("@@@@@ AppManager::startEquipment serverPort = " + QString::number(serverPort));
    netServer->start(serverPort);
    equipmentManager->start();

    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        timer->blockSignals(false);
        debugLog("@@@@@ AppManager::startAll Done");
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

void AppManager::showAuthorizationUsers()
{
    // Обновить список пользователей на экране авторизации
    DBRecordList records = users->getAll();
    userNameModel->update(records);
    const int currentUserCode = Users::getCode(users->getCurrentUser());
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord& r = records[i];
        if(records.count() == 1 || Users::getCode(r) == currentUserCode)
        {
            emit showCurrentUser(i, Users::getDisplayName(r));
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
        users->setCurrentUser(u);
    }

    debugLog("@@@@@ AppManager::stopAuthorization OK");
    db->saveLog(LogType_Warning, LogSource_User, QString("%1. Пользователь: %2. Код: %3").arg(
                    title, login, Tools::intToString(Users::getCode(users->getCurrentUser()))));
    setMainPage(MainPageIndex_Showcase);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::stopAuthorization pause " + Tools::intToString(WAIT_DRAWING_MSEC));
        startAll();
        refreshAll();
        onUserAction();
        if(DB_PATH_MESSAGE) showMessage("БД", Tools::dbPath(DB_PRODUCT_NAME));
        users->clear();
        debugLog("@@@@@ AppManager::stopAuthorization Done");
    });
}

void AppManager::onParseSetRequest(const QString& json, QHash<DBTable*, DBRecordList>& downloadedRecords)
{
    if(settings->insertOrReplace(json)) settings->write();
    if(users->insertOrReplace(json)) users->write();

    for (DBTable* t : db->getTables())
    {
        if(t == nullptr) continue;
        DBRecordList tableRecords = t->parse(json);
        if(tableRecords.count() == 0) continue;
        Tools::debugLog(QString("@@@@@ NetServer::parseSetRequest. Table %1, records %2").
                arg(t->name, QString::number(tableRecords.count())));
        downloadedRecords.insert(t, tableRecords);
    }
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

bool AppManager::onBackgroundDownloadClicked()
{
    if(ENABLE_BACKGROUND_DOWNLOADING) {}
    else showAttention("Фоновая загрузка запрещена");
    return ENABLE_BACKGROUND_DOWNLOADING;
}

void AppManager::onCalendarClosed(const QString& day, const QString& month, const QString& year)
{
    debugLog(QString("@@@@@ AppManager::onCalendarClosed %1.%2.%3").arg(day, month, year));
    QDate d(Tools::stringToInt(year), Tools::stringToInt(month), Tools::stringToInt(day));
    if (d.isValid() && Tools::stringToInt(year) > 2023)
        onSettingInputClosed(SettingCode_VerificationDate, QString("%1.%2.%3").arg(day, month, year));
    else
    {
       showVerificationDateInputPanel();
       showAttention("Неверная дата");
    }
}

void AppManager::onNetCommand(const int command, const QString& param)
{
    debugLog(QString("@@@@@ AppManager::onNetCommand %1 %2").arg(Tools::intToString(command), param));
    netCommandTime = Tools::currentDateTimeToUInt();
    switch (command)
    {
    case NetCommand_Message:
        showAttention(param);
        break;

    case NetCommand_StartLoad:
        emit showDownloadProgress(-1);
        if(ENABLE_BACKGROUND_DOWNLOADING)
        {
            if(Tools::stringToInt(param) != 1) emit showDownloadPanel();
        }
        else
        {
            equipmentManager->pause(true);
            emit showDownloadPanel();
        }
        db->beforeDownloading();
        break;

    case NetCommand_StopLoad:
        netCommandTime = 0;
        emit showDownloadProgress(100);
        if(isRefreshNeeded)
        {
            showAttention("Данные обновлены!");
            isRefreshNeeded = false;
            db->afterDownloading();
            QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { refreshAll(); });
        }
        if(ENABLE_BACKGROUND_DOWNLOADING) {}
        else equipmentManager->pause(false);
        break;

    case NetCommand_Progress:
        emit showDownloadProgress(Tools::stringToInt(param));
        break;

    default:
        break;
    }
}

void AppManager::updateSearch(const QString& value, const bool hierarchyRoot)
{
    searchPanelModel->isHierarchy |= hierarchyRoot;
    debugLog(QString("@@@@@ AppManager::updateSearch %1 %2 %3 %4").arg(
                 value,
                 Tools::boolToString(hierarchyRoot),
                 Tools::boolToString(searchPanelModel->isHierarchy),
                 Tools::intToString(searchPanelModel->filterIndex)));
    emit showSearchHierarchy(searchPanelModel->isHierarchy);
    if(searchPanelModel->isHierarchy)
    {
        searchPanelModel->setHierarchyRoot(hierarchyRoot);
        db->select(DBSelector_GetProductsByGroupCodeIncludeGroups, searchPanelModel->hierarchyLastCode());
    }
    else
    {
        DBSelector s = DBSelector_None;
        switch(searchPanelModel->filterIndex)
        {
        case SearchFilterIndex_Code:
            s = DBSelector_GetProductsByFilteredCode;
            break;
        case SearchFilterIndex_Code2:
            s = DBSelector_GetProductsByFilteredCode2;
            break;
        case SearchFilterIndex_Barcode:
            s = DBSelector_GetProductsByFilteredBarcode;
            break;
        case SearchFilterIndex_Name:
            s = DBSelector_GetProductsByFilteredName;
            break;
        default:
            return;
        }
        db->select(s, value);
    }
}

void AppManager::onSearchClicked()
{
    debugLog("@@@@@ AppManager::onSearchClicked ");
    onUserAction();
    searchPanelModel->isHierarchy = !searchPanelModel->isHierarchy;
    updateSearch("", searchPanelModel->isHierarchy);
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    // Изменился шаблон поиска
    debugLog("@@@@@ AppManager::onSearchFilterEdited " + value);
    searchPanelModel->isHierarchy = false;
    onUserAction();
    updateSearch(value);
}

void AppManager::onSearchResultClicked(const int index)
{
    debugLog("@@@@@ AppManager::onSearchResultClicked " + Tools::intToString(index));
    onUserAction();
    if (index >= 0 && index < searchPanelModel->productCount())
    {
        DBRecord& clicked = searchPanelModel->productByIndex(index);
        if (!ProductDBTable::isGroup(clicked)) setProduct(clicked);
        else if(searchPanelModel->hierarchyDown(clicked)) updateSearch("");
    }
}

void AppManager::onSearchFilterClicked(const int index, const QString& value)
{
    // Изменилось поле поиска (код, штрих-код...)

    debugLog("@@@@@ AppManager::onSearchFilterClicked " + Tools::intToString(index));
    onUserAction();
    searchPanelModel->isHierarchy = false;
    searchPanelModel->filterIndex = index;
    updateSearch(value);
}

void AppManager::onHierarchyUpClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    debugLog("@@@@@ AppManager::onHierarchyUpClicked");
    onUserAction();
    if (searchPanelModel->hierarchyUp()) updateSearch("");
}






