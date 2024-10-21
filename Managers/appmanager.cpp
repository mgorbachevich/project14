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
#include "screenmanager.h"
#include "keyemitter.h"
#include "edituserspanelmodel3.h"
#include "calculator.h"
#include "users.h"
#include "showcase.h"
#include "equipmentmanager.h"
#include "netserver.h"
#include "netactionresult.h"
#include "settings.h"

AppManager::AppManager(QQmlContext* qmlContext, const QSize& screenSize, QApplication *application):
    QObject(application), context(qmlContext)
{
#ifdef REMOVE_DEBUG_LOG_ON_START
    Tools::removeFile(Tools::dbPath(DEBUG_LOG_NAME));
#endif

    debugLog(QString("@@@@@ AppManager::AppManager %1").arg(APP_VERSION));

    screenManager = new ScreenManager(screenSize, this);
    context->setContextProperty("screenManager", screenManager);

    KeyEmitter* keyEmitter = new KeyEmitter(this);
    context->setContextProperty("keyEmitter", keyEmitter);

    settings = new Settings(this);
    users = new Users(this);
    showcase = new Showcase(this);
    db = new DataBase(this);
    netServer = new NetServer(this);
    equipmentManager = new EquipmentManager(this);
    calculator = new Calculator(this);
    timer = new QTimer(this);

    connect(this, &AppManager::start, db, &DataBase::onAppStart);
    connect(netServer, &NetServer::netCommand, this, &AppManager::onNetCommand);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::selectResult, this, &AppManager::onSelectResult);
    connect(equipmentManager, &EquipmentManager::printed, this, &AppManager::onPrinted);
    connect(equipmentManager, &EquipmentManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
    connect(timer, &QTimer::timeout, this, &AppManager::onTimer);

    productPanelModel = new ProductPanelModel(this);
    showcasePanelModel = new ShowcasePanelModel3(this);
    settingsPanelModel = new SettingsPanelModel3(this);
    searchPanelModel = new SearchPanelModel3(screenManager->visibleListRowCount(), this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    viewLogPanelModel = new ViewLogPanelModel(this);
    inputProductCodePanelModel = new InputProductCodePanelModel3(screenManager->visibleListRowCount(), this);
    editUsersPanelModel = new EditUsersPanelModel3(this);
    settingItemModel = new BaseListModel(this);
    calendarDayModel = new BaseListModel(this);
    calendarMonthModel = new BaseListModel(this);
    calendarYearModel = new BaseListModel(this);
    calendarHourModel = new BaseListModel(this);
    calendarMinuteModel = new BaseListModel(this);
    calendarSecondModel = new BaseListModel(this);

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
    context->setContextProperty("calendarHourModel", calendarHourModel);
    context->setContextProperty("calendarMinuteModel", calendarMinuteModel);
    context->setContextProperty("calendarSecondModel", calendarSecondModel);

    QStringList days, months, years, hours, minutes, seconds;
    for (int i = 1; i <= 31; i++) days << Tools::toString(i);
    for (int i = 1; i <= 12; i++) months << Tools::toString(i);
    for (int i = 1; i <= 25; i++) years << Tools::toString(i + 2023);
    for (int i = 0; i <= 23; i++) hours << Tools::toString(i);
    for (int i = 0; i <= 59; i++) minutes << Tools::toString(i);
    for (int i = 0; i <= 59; i++) seconds << Tools::toString(i);
    calendarDayModel->update(days);
    calendarMonthModel->update(months);
    calendarYearModel->update(years);
    calendarHourModel->update(hours);
    calendarMinuteModel->update(minutes);
    calendarSecondModel->update(seconds);

    if(!Tools::checkPermission("android.permission.READ_EXTERNAL_STORAGE") ||
       !Tools::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
        showAttention("Нет разрешений для чтения и записи конфиг.файлов");
    else if(Tools::makeDirs(false, SETTINGS_FILE).isEmpty() ||
            Tools::makeDirs(false, USERS_FILE).isEmpty())
        showAttention("Не созданы папки конфиг.файлов");

#ifdef REMOVE_SETTINGS_FILE_ON_START
    Tools::removeFile(SETTINGS_FILE);
#endif

    settings->read();
    updateSettings(0);
    equipmentManager->create();

    onUserAction();
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { emit start(); });
    debugLog("@@@@@ AppManager::AppManager Done");
}

AppManager::~AppManager()
{
    Tools::debugLog("@@@@@ AppManager::~AppManager");
    stopAll(false);
}

void AppManager::onDBStarted()
{
    debugLog("@@@@@ AppManager::onDBStarted");
    onUserAction();
    setSettingsInfo();
    setSettingsNetInfo();
    updateSettings();

    if(db->isStarted())
    {
        startAuthorization();
        timer->start(APP_TIMER_MSEC);
    }
    debugLog("@@@@@ AppManager::onDBStarted Done");
}

void AppManager::onTimer()
{
#ifdef DEBUG_ONTIMER_MESSAGE
        debugLog("@@@@@ AppManager::onTimer " + Tools::toString((int)(status.userActionTime / 1000)));
#endif
#ifdef DEBUG_MEMORY_MESSAGE
        Tools::debugMemory();
#endif
    const quint64 now = Tools::nowMsec();

    if(isAuthorizationOpened()) // Авторизация
    {
        status.isAlarm = false;
        updateSystemStatus();
        setExternalDisplay();
    }
    else if(isSettingsOpened()) // Настройки
    {
        status.isAlarm = false;
        setExternalDisplay();
    }
    else
    {
        // Сброс товара при бездействии:
        if (isProduct() && settings->getIntValue(SettingCode_ProductReset, true) == ProductReset_Time)
        {
            quint64 waitReset = settings->getIntValue(SettingCode_ProductResetTime); // секунды
            if(waitReset > 0 && waitReset * 1000 < now - status.userActionTime)
            {
                debugLog("@@@@@ AppManager::onTimer reset product");
                resetProduct();
            }
        }
        // Блокировка:
        quint64 waitBlocking = settings->getIntValue(SettingCode_Blocking); // минуты
        if(waitBlocking > 0 && waitBlocking * 1000 * 60 < now - status.userActionTime)
        {
            debugLog("@@@@@ AppManager::onTimer blocking");
            status.userActionTime = now;
            startAuthorization();
        }
        else update();
    }

    // Ожидание окончания сетевых запросов:
    if(netServer->isStarted() && status.netActionTime > 0 &&
            WAIT_NET_COMMAND_MSEC < now - status.netActionTime)
    {
        debugLog("@@@@@ AppManager::onTimer netActionTime");
        onNetCommand(NetCommand_StopLoad, Tools::toString(false));
    }
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    // Настройка изменилась
    debugLog(QString("@@@@@ AppManager::onSettingInputClosed %1 %2").arg(
                 QString::number(settingItemCode), value));
    DBRecord* r = settings->getByCode(settingItemCode);
    if (r == nullptr)
    {
        showAttention("Ошибка настройки (неизвестная запись)!");
        return;
    }
    if(!settings->setValue(settingItemCode, value)) return;
    updateSettings(settings->getCurrentGroupCode());
    settings->write();
    QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(
                QString::number(settingItemCode), value);
    db->saveLog(LogType_Warning, LogSource_Admin, s);
}

void AppManager::onPiecesInputClosed(const QString &value)
{
    debugLog("@@@@@ AppManager::onPiecesInputClosed " + value);
    onUserAction();
    int v = Tools::toInt(value);
    const int maxChars = settings->getIntValue(SettingCode_CharNumberPieces);
    if(value.length() > maxChars)
    {
        v = Tools::toInt(value.leftJustified(maxChars));
        showAttention("Максимальная длина " + Tools::toString(maxChars));
    }
    if(v < 1)
    {
        v = 1;
        showAttention("Количество не должно быть меньше 1");
    }
    status.pieces = v;
    update();
}

void AppManager::onSelectResult(const DBSelector selector, const DBRecordList& records, const bool ok)
{
    // Получен результ из БД

    showWait(false);
    debugLog("@@@@@ AppManager::onSelectResult " + QString::number(selector));
    if (!ok)
    {
        showAttention("Ошибка базы данных!");
        return;
    }

    switch(selector)
    {

    case DBSelector_GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DBSelector_GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        //showFoundProductsToast(records.count());
        emit showHierarchyRoot(searchPanelModel->isHierarchyRoot());
        emit showControlParam(ControlParam_SearchTitle, searchPanelModel->hierarchyTitle());
        searchPanelModel->update(records, -1);
        break;

    case DBSelector_SetProductByCode2: // Отображение товара с кодом:
        if(records.isEmpty())
        {
            showAttention("Товар не найден!");
            break;
        }
        if(records.count() > 1)
        {
            showAttention("Найдено несколько товаров с таким номером!");
            break;
        }
        emit closeInputProductPanel();
        setProduct(records.at(0));
        break;

    case DBSelector_GetProductsByInputCode: // Отображение товаров с фрагментом кода:
    {
        showFoundProductsToast(records.count());
        inputProductCodePanelModel->update(records);

        // Enable/disable button "Continue":
        DBRecord r = db->selectByCode(DBTABLENAME_PRODUCTS, inputProductCodePanelModel->descriptor.param);
        emit enableSetProductByInputCode(!r.isEmpty());
        break;
    }

    case DBSelector_GetProductsByFilteredCode: // Отображение товаров с фрагментом кода:
        showFoundProductsToast(records.count());
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по коду");
        searchPanelModel->update(records, SearchFilterIndex_Code);
        break;

    case DBSelector_GetProductsByFilteredCode2: // Отображение товаров с фрагментом номера:
        showFoundProductsToast(records.count());
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по номеру");
        searchPanelModel->update(records, SearchFilterIndex_Code2);
        break;

    case DBSelector_GetProductsByFilteredBarcode: // Отображение товаров с фрагментом штрих-кода:
        showFoundProductsToast(records.count());
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по штрих-коду");
        searchPanelModel->update(records, SearchFilterIndex_Barcode);
        break;

    case DBSelector_GetProductsByFilteredName: // Отображение товаров с фрагментом наименования:
        showFoundProductsToast(records.count());
        emit showControlParam(ControlParam_SearchTitle, "Поиск товаров по наименованию");
        searchPanelModel->update(records, SearchFilterIndex_Name);
        break;

   case DBSelector_GetShowcaseProducts: // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        db->select(DBSelector_GetShowcaseResources, records);
        emit showShowcaseSort(status.productSort, status.isProductSortIncrement);
        break;

    case DBSelector_GetShowcaseResources: // Отображение картинок товаров экрана Showcase:
    {
        debugLog("@@@@@ AppManager::onSelectResult GetShowcaseResources"  + QString::number(records.count()));
        QStringList fileNames;
        foreach (auto r, records)
        {
            QString s = getImageFileWithQmlPath(r);
            // debugLog("@@@@@ AppManager::onSelectResult GetShowcaseResources" << s;
            fileNames.append(s);
        }
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DBSelector_GetAllLabels:
    {
        const SettingCode settingCode = SettingCode_PrintLabelFormat;
        DBRecord& r = *settings->getByCode(settingCode);
        QStringList names = settings->getValueList(r);
        if(records.count() > 0)
        {
            names.removeAll("");
            foreach (auto ri, records)
            {
                const QString s = ri[ResourceDBTable::Name].toString();
                if(!names.contains(s)) names.append(s);
            }
            r[SettingField_ValueList] = Tools::toString(names);
        }
        int n = settings->getIntValue(settingCode, true);
        if(n < 0 || n >= names.count()) r[SettingField_Value] = "0";
        break;
    }

    default: break;
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

void AppManager::onVirtualKeyboardSet(const int v)
{
    debugLog("@@@@@ AppManager::onVirtualKeyboardSet " + Tools::toString(v));
    onUserAction();
    emit showVirtualKeyboard(v);
}

void AppManager::onCalendarClosed(const int settingItemCode,
                                  const QString& day, const QString& month, const QString& year,
                                  const QString& hour, const QString& minute, const QString& second)
{
    debugLog(QString("@@@@@ AppManager::onCalendarClosed %1.%2.%3 %4:%5:%6").arg(
                 day, month, year, hour, minute, second));
    QDate d(Tools::toInt(year), Tools::toInt(month), Tools::toInt(day));
    QTime t(Tools::toInt(hour), Tools::toInt(minute), Tools::toInt(second));
    if (d.isValid() && d.year() > 2023)
    {
        switch(settingItemCode)
        {
        case SettingCode_DateTime:
        {
            if(t.isValid())
            {
                QString s = QDateTime(d, t).toString(DATE_TIME_LONG_FORMAT);
                showConfirmation(ConfirmSelector_SetDateTime, "Установить дату и время? " + s, s);
                return;
            }
            break;
        }
        case SettingCode_VerificationDate:
        {
            QString s = d.toString(DATE_FORMAT);
            showConfirmation(ConfirmSelector_SetVerificationDate, "Установить дату поверки? " + s, s);
            return;
        }}
    }
    showDateInputPanel(settingItemCode);
    showAttention("Неверная дата");
}

void AppManager::onConfirmationClicked(const int selector, const QString& param)
{
    debugLog(QString("@@@@@ AppManager::onConfirmationClicked %1 %2").arg(Tools::toString(selector), param));
    onUserAction();
    switch (selector)
    {
    case ConfirmSelector_SetDateTime:
    {
        onSettingInputClosed(SettingCode_DateTime, param);
        equipmentManager->setSystemDateTime(Tools::dateTimeFromString(param));
        break;
    }

    case ConfirmSelector_SetVerificationDate:
        onSettingInputClosed(SettingCode_VerificationDate, param);
       break;

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

    case ConfirmSelector_RemoveFromShowcase:
        showcase->removeByCode(product[ProductDBTable::Code].toString());
        emit setCurrentProductFavorite(isProductInShowcase(product));
        updateShowcase();
        break;

    case ConfirmSelector_AddToShowcase:
        showcase->insertOrReplaceRecord(showcase->createRecord(product[ProductDBTable::Code].toString()));
        emit setCurrentProductFavorite(isProductInShowcase(product));
        updateShowcase();
        break;
    }
}

bool AppManager::isProductInShowcase(const DBRecord& product)
{
    return showcase->getByCode(product[0].toInt()) != nullptr;
}

void AppManager::onSettingsItemClicked(const int index)
{
    onUserAction();
    DBRecord* r = settings->getByIndexInCurrentGroup(index);
    if(r == nullptr || r->empty())
    {
        debugLog("@@@@@ AppManager::onSettingsItemClicked ERROR " + Tools::toString(index));
        return;
    }

    const int code = Settings::getCode(*r);
    const QString& name = Settings::getName(*r);
    const int type = Settings::getType(*r);
    debugLog(QString("@@@@@ AppManager::onSettingsItemClicked %1 %2 %3").arg(
                 Tools::toString(code), name, QString::number(type)));

    switch (code) // SettingType_Custom
    {
    case SettingCode_ClearLog:
        clearLog();
        return;

    case SettingCode_Equipment:
    case SettingCode_WiFi:
    case SettingCode_Ethernet:
        settings->nativeSettings(code);
        return;

    case SettingCode_SystemSettings:
        emit showPasswordInputBox(code);
        return;

    case SettingCode_Users:
        onClicked(Clicked_EditUsers);
        return;

    case SettingCode_DateTime:
    case SettingCode_VerificationDate:
        showDateInputPanel(code);
        return;

    case SettingCode_Group_Info:
        setSettingsNetInfo();
        settings->write();
        break;

    default: break;
    }

    switch (type)
    {
    case SettingType_Custom:
        return;

    case SettingType_Group:
        updateSettings(code);
        emit showSettingsPanel(settings->getCurrentGroupName());
        break;

    case SettingType_InputNumber:
    case SettingType_InputText:
        emit showSettingInputBox(code, name, settings->getStringValue(*r));
        break;

    case SettingType_List:
        settingItemModel->update(Settings::getValueList(*r));
        showSettingComboBox2(*r);
        break;

    case SettingType_IntervalNumber:
    {
        QStringList list = Settings::getValueList(*r);
        if(list.count() >= 2)
        {
            int from = Tools::toInt(list[0]);
            int to = Tools::toInt(list[1]);
            int value = settings->getIntValue(*r);
            emit showSettingSlider(code, name, from, to, 1, value);
        }
        break;
    }

    case SettingType_Unused:
    case SettingType_UnusedGroup:
        showMessage(name, "Не поддерживается");
        break;

    case SettingType_ReadOnly:
        showMessage(name, "Редактирование запрещено");
        break;

    case SettingType_GroupWithPassword:
        emit showPasswordInputBox(code);

    default: break;
    }
}

void AppManager::clearLog()
{
    debugLog("@@@@@ AppManager::clearLog");
    showConfirmation(ConfirmSelector_ClearLog, "Вы хотите очистить лог?", "");
}

void AppManager::showDateInputPanel(const int settingCode)
{
    const SettingCode code = (SettingCode)settingCode;
    switch(code)
    {
    case SettingCode_DateTime:
    {
        QDateTime dt = Tools::now();
        QDate d = dt.date();
        QTime t = dt.time();
        debugLog(QString("@@@@@ AppManager::showDateInputPanel %1.%2.%3 %4:%5:%6").arg(
                     Tools::toString(d.day()), Tools::toString(d.month()),Tools::toString(d.year()),
                     Tools::toString(t.hour()), Tools::toString(t.minute()), Tools::toString(t.second())));
        emit showCalendarPanel(code, settings->getName(code),
                               d.day(), d.month(), d.year(), t.hour(), t.minute(), t.second());
        break;
    }
    case SettingCode_VerificationDate:
    {
        QDate d = QDate::fromString(settings->getStringValue(code), DATE_FORMAT);
        debugLog(QString("@@@@@ AppManager::showDateInputPanel %1.%2.%3").arg(
                     Tools::toString(d.day()), Tools::toString(d.month()),Tools::toString(d.year())));
        emit showCalendarPanel(code, settings->getName(code), d.day(), d.month(), d.year(), -1, -1, -1);
        break;
    }
    default: break;
    }
}

void AppManager::updateSettings(const int groupCode)
{
    debugLog("@@@@@ AppManager::updateSettings ");
    settings->update(groupCode);
    settingsPanelModel->update(*settings);
}

void AppManager::setMainPage(const int i)
{
    debugLog("@@@@@ AppManager::setMainPage " + Tools::toString(i));
    screenManager->mainPageIndex = i;
    emit showMainPage(screenManager->mainPageIndex);
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
    status.isSettings = true;
    stopAll();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    setExternalDisplay();
    emit showSettingsPanel(settings->getCurrentGroupName());
}

void AppManager::stopSettings()
{
    debugLog("@@@@@ AppManager::stopSettings");
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        startAll();
        refreshAll();
        status.isSettings = false;
        if(users->getByName(users->getName(getCurrentUser())).isEmpty()) // Пользователь сменился
            startAuthorization();
        else setExternalDisplay();
    });
}

void AppManager::startAuthorization()
{
    debugLog("@@@@@ AppManager::startAuthorization");
    stopAll();
    setMainPage(MainPageIndex_Authorization);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::startAuthorization pause " + Tools::toString(WAIT_DRAWING_MSEC));
        updateSystemStatus();
        db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
        showAuthorizationUsers();
        emit showControlParam(ControlParam_AuthorizationTitle1, QString("%1   № %2").arg(
                                  settings->getStringValue(SettingCode_Model),
                                  settings->getStringValue(SettingCode_SerialNumber)));
        emit showControlParam(ControlParam_AuthorizationTitle2, "(III)  " + equipmentManager->getWMDescription());
        emit showControlParam(ControlParam_AuthorizationTitle3, QString("Поверка %1").arg(
                                  settings->getStringValue(SettingCode_VerificationDate)));
        setExternalDisplay();
        debugLog("@@@@@ AppManager::startAuthorization Done");
    });
}

void AppManager::stopAuthorization(const QString& login, const QString& password)
{
    const QString title = "Авторизация";

#ifdef CHECK_AUTHORIZATION
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
#endif

    debugLog("@@@@@ AppManager::stopAuthorization OK");
    db->saveLog(LogType_Warning, LogSource_User, QString("%1. Пользователь: %2. Код: %3").arg(
                    title, login, Tools::toString(Users::getCode(getCurrentUser()))));
    setMainPage(MainPageIndex_Showcase);
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        debugLog("@@@@@ AppManager::stopAuthorization pause " + Tools::toString(WAIT_DRAWING_MSEC));
        startAll();
        refreshAll();
        onUserAction();
#ifdef DB_PATH_MESSAGE
        showMessage("БД", Tools::dbPath(DB_PRODUCT_NAME));
#endif
        users->clear();
        setExternalDisplay();
        debugLog("@@@@@ AppManager::stopAuthorization Done");
    });
}

void AppManager::updateShowcase()
{
    showWait(true);
    showcase->getAll();
    showWeightFlags();
    db->select(DBSelector_GetShowcaseProducts,
               Tools::toString(status.productSort),
               Tools::toString(status.isProductSortIncrement));
}

void AppManager::refreshAll()
{
    // Обновить всё на экране
    debugLog("@@@@@ AppManager::refreshAll");
    resetProduct();
    setMainPage(screenManager->mainPageIndex);
    updateShowcase();
    emit showVirtualKeyboard(-1);
    searchPanelModel->refresh();
    updateSearch();
}

void AppManager::showToast(const QString &text, const int delaySec)
{
    if(!text.isEmpty())
    {
        emit hideToastBox();
        debugLog(QString("@@@@@ AppManager::showToast %1").arg(text));
        emit showToastBox(text);
        QTimer::singleShot(delaySec * 1000, this, [this]() { emit hideToastBox(); } );
    }
}

void AppManager::showWait(const bool v)
{
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this, v]()
    {
        status.isWaiting = v;
        emit showWaitBox(v);
    });
}

void AppManager::updateSystemStatus()
{
    QString dateTime = Tools::dateTimeFromUInt(Tools::nowMsec(), "%1 %2", "dd.MM", "hh:mm");
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
    emit showMessageBox(title, text);
}

void AppManager::showConfirmation(const ConfirmSelector selector, const QString &text, const QString &param)
{
    debugLog(QString("@@@@@ AppManager::showConfirmation %1 %2 %3").arg(QString::number(selector), text, param));
    emit showConfirmationBox(selector, "Подтверждение", text, param);
}

void AppManager::netDownload(QHash<DBTable*, DBRecordList> rs, int &successCount, int &errorCount)
{
    status.netActionTime = Tools::nowMsec();
    db->netDownload(rs, successCount, errorCount);
}

QString AppManager::netDelete(const QString &tableName, const QString &codes)
{
    status.netActionTime = Tools::nowMsec();
    return db->netDelete(tableName, codes);
}

QString AppManager::netUpload(const QString &t, const QString &s, const bool b)
{
    status.netActionTime = Tools::nowMsec();
    return db->netUpload(settings, users, t, s, b);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    if(!product.isEmpty())
    {
        QString code = product[ProductDBTable::Code].toString();
        debugLog("@@@@@ AppManager::setProduct " + code);
        emit showProductPanel(product[ProductDBTable::Name].toString(), Calculator::isPiece(product));
        productPanelModel->update(product, (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
        showTareToast(setProductTare() || equipmentManager->isTareFlag());
        db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(code));
        QString pictureCode = product[ProductDBTable::PictureCode].toString();
        DBRecord r = db->selectByCode(DBTABLENAME_PICTURES, pictureCode);
        QString imagePath = r.count() > 0 ? getImageFileWithQmlPath(r) : DUMMY_IMAGE_FILE;
        emit showControlParam(ControlParam_ProductImage, imagePath);
        debugLog("@@@@@ AppManager::onSelectResult showProductImage " + imagePath);
        emit setCurrentProductFavorite(isProductInShowcase(product));
        update();
    }
}
void AppManager::showFoundProductsToast(const int v)
{
    // if(v > 1) showToast(QString("Найдено товаров: %1").arg(Tools::toString(v)));
}

void AppManager::resetProduct(const bool show) // Сбросить выбранный продукт
{
    isResetProductNeeded = false;
    if(isProduct())
    {
        debugLog("@@@@@ AppManager::resetProduct");
        product.clear();
        status.onResetProduct();
        emit resetCurrentProduct();
    }
    if(show) update();
}

void AppManager::onUserAction()
{
    debugLog("@@@@@ AppManager::onUserAction");
#ifdef DEBUG_MEMORY_MESSAGE
    Tools::debugMemory();
#endif
    status.onUserAction();
}

void AppManager::onPrinted(const DBRecord& newTransaction)
{
    // Принтер ответил что этикетка напечатана

    debugLog("@@@@@ AppManager::onPrinted");
    db->saveLog(LogType_Error, LogSource_Print, QString("Печать. Код товара: %1. Вес/Количество: %2").arg(
                newTransaction[TransactionDBTable::ItemCode].toString(),
                newTransaction[TransactionDBTable::Weight].toString()));
    db->saveTransaction(newTransaction);
    if (settings->getIntValue(SettingCode_ProductReset, true) == ProductReset_Print)
        isResetProductNeeded = true;
}

void AppManager::onEquipmentParamChanged(const EquipmentParam param,
                                         const int errorCode,
                                         const QString& errorDescription)
{
    // Изменился параметр оборудования
#ifdef DEBUG_WEIGHT_STATUS
    debugLog(QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(
                 Tools::toString(param), Tools::toString(errorCode)));
#endif

    switch (param)
    {
    case EquipmentParam_None: return;

    case EquipmentParam_WeightValue:
        if(isResetProductNeeded) resetProduct();
        break;

    case EquipmentParam_WeightError:
        if(isResetProductNeeded) resetProduct();
        db->saveLog(LogType_Error, LogSource_Weight, QString("Ошибка весового модуля. Код: %1. Описание: %2").arg(
                    QString::number(errorCode), errorDescription));
        break;

    case EquipmentParam_PrintError:
    {
        db->saveLog(LogType_Error, LogSource_Print, QString("Ошибка принтера. Код: %1. Описание: %2").arg(
                    QString::number(errorCode), errorDescription));
        emit showControlParam(ControlParam_PrinterStatus, errorCode == 0 ? "" : errorDescription);
        break;
    }
    }
    update();
}

void AppManager::alarm()
{
    if(status.isAlarm) return;
    auto future = QtConcurrent::run([this]
    {
        status.isAlarm = true;
        while(status.isAlarm)
        {
            beepSound();
            Tools::pause(ALARM_PAUSE_MSEC);
        }
    });
}

void AppManager::beepSound()
{
    Tools::sound("qrc:/Sound/KeypressInvalid.mp3", settings->getIntValue(SettingCode_KeyboardSoundVolume));
}

void AppManager::clickSound()
{
    Tools::sound("qrc:/Sound/KeypressStandard.mp3", settings->getIntValue(SettingCode_KeyboardSoundVolume));
}

DBRecord &AppManager::getCurrentUser() { return users->getCurrentUser(); }

void AppManager::debugLog(const QString& s)
{
    Tools::debugLog(s);
}

bool AppManager::isAdmin() { return Users::isAdmin(users->getCurrentUser()); }

bool AppManager::isAuthorizationOpened()
{
    return screenManager->mainPageIndex == MainPageIndex_Authorization;
}

void AppManager::onPopupOpened(const bool open)
{
    debugLog(QString("@@@@@ AppManager::onPopupOpened %1 %2").arg(
                 Tools::toString(open), Tools::toString(screenManager->popupCount)));
    if(open) screenManager->popupCount++;
    else if((--(screenManager->popupCount)) < 1)
    {
        screenManager->popupCount = 0;
        if(!isSettingsOpened()) setMainPage(screenManager->mainPageIndex);
    }
}

void AppManager::stopAll(const bool show)
{
    debugLog("@@@@@ AppManager::stopAll");
    timer->blockSignals(true);
    resetProduct(show);
    netServer->stop();
    equipmentManager->stop();
    status.onStopAll();
    debugLog("@@@@@ AppManager::stopAll Done");
}

void AppManager::startAll()
{
    debugLog("@@@@@ AppManager::startAll");

    const int serverPort = settings->getIntValue(SettingCode_TCPPort);
    debugLog("@@@@@ AppManager::startAll serverPort = " + QString::number(serverPort));
    netServer->start(serverPort);
    equipmentManager->start();
    status.autoPrintMode = settings->getBoolValue(SettingCode_PrintAuto) ? AutoPrintMode_Disabled : AutoPrintMode_Off;

    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]()
    {
        timer->blockSignals(false);
        debugLog("@@@@@ AppManager::startAll Done");
    });
}

void AppManager::onEditUsersPanelClose()
{
    debugLog("@@@@@ AppManager::onEditUsersPanelClose ");
    users->clear();
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
    const int currentUserCode = Users::getCode(getCurrentUser());
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

void AppManager::onParseSetRequest(const QString& json, QHash<DBTable*, DBRecordList>& downloadedRecords)
{
    if(settings->insertOrReplace(json)) settings->write();
    if(users->insertOrReplace(json)) users->write();
    if(showcase->insertOrReplace(json)) showcase->write();

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
    case SettingCode_Group_FactorySettings:
        if(inputPassword == settings->getConfigValue(ScaleConfigField_FactorySettingsPassword))
        {
            updateSettings(code);
            emit showSettingsPanel(settings->getCurrentGroupName());
        }
        else showAttention("Неверный пароль");
        break;

    case SettingCode_SystemSettings:
        if(inputPassword == settings->getConfigValue(ScaleConfigField_SystemSettingsPassword))
            settings->nativeSettings(code);
        else showAttention("Неверный пароль");
        break;

    default: break;
    }
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    // Изменился шаблон поиска
    debugLog("@@@@@ AppManager::onSearchFilterEdited " + value);
    onUserAction();
    searchPanelModel->isHierarchy = false;
    searchPanelModel->isRoot = false;
    searchPanelModel->descriptor.reset(value);
    updateSearch();
}

void AppManager::onSearchFilterClicked(const int index, const QString& value)
{
    // Изменилось поле поиска (код, штрих-код...)

    debugLog("@@@@@ AppManager::onSearchFilterClicked " + Tools::toString(index));
    onUserAction();
    searchPanelModel->isHierarchy = false;
    searchPanelModel->isRoot = false;
    searchPanelModel->filterIndex = index;
    searchPanelModel->descriptor.reset(value);
    updateSearch();
}

void AppManager::onNetCommand(const int command, const QString& param)
{
    debugLog(QString("@@@@@ AppManager::onNetCommand %1 %2").arg(Tools::toString(command), param));
    status.netActionTime = Tools::nowMsec();
    switch (command)
    {
    case NetCommand_Message:
        showAttention(param);
        break;

    case NetCommand_StartLoad:
        emit showDownloadProgress(-1);
        equipmentManager->pause(true);
        DataBase::removeDBFile(DB_PRODUCT_COPY_NAME);
        DataBase::copyDBFile(DB_PRODUCT_NAME, DB_PRODUCT_COPY_NAME);
        emit showDownloadPanel();
        break;

    case NetCommand_StopLoad:
        status.netActionTime = 0;
        emit showDownloadProgress(100);
        if(status.isRefreshNeeded)
        {
            status.isRefreshNeeded = false;
            QString toast;
            if(Tools::toBool(param)) // Ok
            {
                status.downloadDateTime = Tools::now().toString(DATE_TIME_LONG_FORMAT);
                QString s = status.downloadDateTime;
                if(status.downloadedRecords > 0)
                    s += QString(" (Записи %1)").arg(Tools::toString(status.downloadedRecords));
                settings->setValue(SettingCode_InfoLastDownload, s);
                updateSettings();
                //s = "Загрузка успешно завершена. Данные обновлены!";
            }
            else // Timeout
            {
                if(DataBase::isDBFileExists(DB_PRODUCT_COPY_NAME))
                {
                    toast += "ОШИБКИ ПРИ ЗАГРУЗКЕ! ДАННЫЕ НЕ ОБНОВЛЕНЫ!";
                    const QString& ld = settings->getStringValue(SettingCode_InfoLastDownload);
                    if(!ld.isEmpty()) toast += "\nПоследняя успешная загрузка\n" + ld;
                    DataBase::renameDBFile(DB_PRODUCT_COPY_NAME, DB_PRODUCT_NAME);
                }
                else toast = "Ошибки при загрузке! Возможна потеря данных! Данные обновлены!";
            }
            showToast(toast, SHOW_LONG_TOAST_SEC);
            QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { refreshAll(); });
        }
        DataBase::removeDBFile(DB_PRODUCT_COPY_NAME);
        equipmentManager->pause(false);
        break;

    case NetCommand_Progress:
        emit showDownloadProgress(Tools::toInt(param));
        break;

    default: break;
    }
}

void AppManager::onNetResult(NetActionResult& result)
{
    debugLog("@@@@@ AppManager::onNetResult");
    QString toast;
    switch(result.rule)
    {
    case RouterRule_Delete:
        toast = "Данные удалены";
        if(status.isRefreshNeeded)
        {
            status.isRefreshNeeded = false;
            QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { refreshAll(); });
        }
        break;

    case RouterRule_Get:
        toast = "Данные выгружены";
        break;

    case RouterRule_Set: // -> onNetCommand
    default: return;
    }
    if(result.errorCount > 0) toast += QString(". ОШИБКИ: %1").arg(Tools::toString(result.errorCount));
    showToast(toast);
}

void AppManager::onInputProductCodeEdited(const QString &value)
{
    debugLog("@@@@@ AppManager::onInputProductCodeEdited " + value);
    inputProductCodePanelModel->descriptor.reset(value);
    updateInputCodeList();
}

void AppManager::updateInputCodeList()
{
    debugLog(inputProductCodePanelModel->log("AppManager::updateInputCodeList"));
    showWait(true);
    db->select(DBSelector_GetProductsByInputCode,
               inputProductCodePanelModel->descriptor.param,
               Tools::toString(status.lastProductSort),
               inputProductCodePanelModel->descriptor.firstLoadRow,
               inputProductCodePanelModel->loadLimit());
}

void AppManager::updateSearch()
{
    debugLog(searchPanelModel->log("AppManager::updateSearch(1)"));
    searchPanelModel->isHierarchy |= searchPanelModel->isRoot;
    DBSelector selector = DBSelector_None;
    if(searchPanelModel->isHierarchy)
    {
        searchPanelModel->setHierarchyRoot(searchPanelModel->isRoot);
        selector = DBSelector_GetProductsByGroupCodeIncludeGroups;
        searchPanelModel->descriptor.param = searchPanelModel->hierarchyLastCode();
    }
    else
    {
        switch(searchPanelModel->filterIndex)
        {
        case SearchFilterIndex_Code:    selector = DBSelector_GetProductsByFilteredCode; break;
        case SearchFilterIndex_Code2:   selector = DBSelector_GetProductsByFilteredCode2; break;
        case SearchFilterIndex_Barcode: selector = DBSelector_GetProductsByFilteredBarcode; break;
        case SearchFilterIndex_Name:    selector = DBSelector_GetProductsByFilteredName; break;
        default: return;
        }
    }
    showWait(true);
    debugLog(searchPanelModel->log("AppManager::updateSearch(2)"));
    emit showSearchHierarchy(searchPanelModel->isHierarchy);
    db->select(selector,
               searchPanelModel->descriptor.param, "",
               searchPanelModel->descriptor.firstLoadRow,
               searchPanelModel->loadLimit());
}

bool AppManager::isProduct()
{
    return product.count() >= db->getTable(DBTABLENAME_PRODUCTS)->columnCount();
}

int AppManager::showcaseCount()
{
    return showcase->count();
}

DBRecord AppManager::getShowcaseProductByIndex(const int i)
{
    return showcase->getByIndex(i);
}

void AppManager::setSettingsInfo()
{
    debugLog("@@@@@ AppManager::setSettingsInfo ");
#ifdef Q_OS_ANDROID
    QString s;
    QStringList ps = { "android.permission.ACCESS_WIFI_STATE",
                       "android.permission.CHANGE_WIFI_STATE",
                       "android.permission.ACCESS_FINE_LOCATION",
                       "android.permission.ACCESS_COARSE_LOCATION",
                       "android.permission.ACCESS_BACKGROUND_LOCATION",
                       "android.permission.ACCESS_NETWORK_STATE" };
    foreach(const QString& p, ps) if(!Tools::checkPermission(p)) s += "\n" + p;
    if(!s.isEmpty()) showMessage("Нет разрешения", s);
#endif
    equipmentManager->start();
    settings->setValue(SettingCode_InfoVersion,       APP_VERSION);
    settings->setValue(SettingCode_InfoServerVersion, netServer->version());
    settings->setValue(SettingCode_InfoDBVersion,     db->version());
    settings->setValue(SettingCode_InfoMODVersion,    equipmentManager->MODVersion());
    settings->setValue(SettingCode_InfoDaemonVersion, equipmentManager->daemonVersion());
    settings->setValue(SettingCode_InfoWMVersion,     equipmentManager->WMVersion());
    settings->setValue(SettingCode_InfoPMVersion,     equipmentManager->PMVersion());
    settings->setValue(SettingCode_InfoAndroidBuild,  Tools::getAndroidBuild());
    settings->setValue(SettingCode_InfoBluetooth,     "Не поддерживается");
    equipmentManager->stop();
}

void AppManager::setSettingsNetInfo()
{
    debugLog("@@@@@ AppManager::setSettingsNetInfo ");
    settings->setValue(SettingCode_InfoIP,       Tools::getIP());
    settings->setValue(SettingCode_InfoWiFiSSID, Tools::getSSID());
}

void AppManager::showSettingComboBox2(const DBRecord& r)
{
    emit showSettingComboBox(Settings::getCode(r),
                             Settings::getName(r),
                             settings->getIntValue(r, true),
                             settings->getStringValue(r),
                             settings->getComment(r));
}

void AppManager::showTareToast(const bool showZero)
{
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this, showZero]()
    {
        if (showZero || equipmentManager->isTareFlag())
            showToast(QString("Тара %1 кг").arg(equipmentManager->getTareAsString()));
    });
}

void AppManager::updateSettings() // При старте и после загрузки
{
    debugLog("@@@@@ AppManager::updateSettings ");
    db->select(DBSelector_GetAllLabels);
    Calculator::update(settings);
    settings->write();
}

void AppManager::showWeightFlags()
{
    emit showControlParam(ControlParam_ZeroFlag,  equipmentManager->isZeroFlag() ?    ICON_ZERO_ON : ICON_ZERO_OFF);
    emit showControlParam(ControlParam_TareFlag,  equipmentManager->isTareFlag() ?    ICON_TARE_ON : ICON_TARE_OFF);
    emit showControlParam(ControlParam_FixedFlag, equipmentManager->isWeightFixed() ? ICON_FIX_ON :  ICON_FIX_OFF);

    QString icon = ICON_WM_ON;
    if(!equipmentManager->isWM()) icon = ICON_WM_OFF;
    else if(equipmentManager->getWMMode() == EquipmentMode_None) icon = ICON_WM_NONE;
    else if(equipmentManager->isWMError()) icon = ICON_WM_ERROR;
    else if (status.autoPrintMode == AutoPrintMode_On) icon = ICON_AUTO_ON;
    else if (status.autoPrintMode == AutoPrintMode_Disabled) icon = ICON_AUTO_OFF;
    emit showControlParam(ControlParam_WeightErrorOrAutoPrintIcon, icon);
}

void AppManager::update()
{
#ifdef DEBUG_WEIGHT_STATUS
    debugLog("@@@@@ AppManager::update");
#endif
    const bool isPieceProduct = isProduct() && Calculator::isPiece(product);
    const int oldPieces = status.pieces;
    if(isPieceProduct && status.pieces < 1) status.pieces = 1;
    const bool isWM = equipmentManager->isWM() && equipmentManager->getWMMode() != EquipmentMode_None;
    const bool isPM = equipmentManager->isPM();
    const bool isZero = equipmentManager->isZeroFlag();
    //const bool isTare = equipmentManager->isTareFlag();
    const bool isFixed = equipmentManager->isWeightFixed();
    const bool isWMError = equipmentManager->isWMError() || !isWM;
    const bool isAutoPrint = status.autoPrintMode == AutoPrintMode_On &&
            (!isPieceProduct || settings->getBoolValue(SettingCode_PrintAutoPcs));

    status.quantity = NO_DATA;
    status.price = NO_DATA;
    status.amount = NO_DATA;
    //status.tare = isTare ? equipmentManager->getTareAsString() : NO_DATA;

    // Проверка флага 0 - новый товар на весах (начинать обязательно с этого!):
    if (isZero) status.isPrintCalculateMode = true;

    // Рисуем флажки:
    showWeightFlags();

    // Рисуем загаловки:
    QString wt = isPieceProduct ? "КОЛИЧЕСТВО, шт" : "МАССА, кг";
    if(equipmentManager->getWMMode() == EquipmentMode_Demo) wt += " ДЕМО";
    QString pt = "ЦЕНА, руб";
    if(isProduct())
    {
        if (isPieceProduct) pt += "/шт";
        else pt += Calculator::is100gBase(product) ? "/100г" : "/кг";
    }
    emit showControlParam(ControlParam_WeightTitle, wt);
    emit showControlParam(ControlParam_PriceTitle, pt);
    emit showControlParam(ControlParam_AmountTitle, "СТОИМОСТЬ, руб");

    // Рисуем количество (вес/штуки):
    if(isWM)
    {
        if(equipmentManager->isWMOverloaded())
        {
            showToast("ПЕРЕГРУЗКА!");
            alarm();
        }
        else
        {
            status.isAlarm = false;
            if(isPieceProduct || !isWMError) status.quantity = Calculator::quantity(product, equipmentManager);
        }
    }
    emit showControlParam(ControlParam_WeightValue, status.quantity);
    emit showControlParam(ControlParam_WeightColor, isPieceProduct || (isFixed && !isWMError) ? COLOR_ACTIVE : COLOR_PASSIVE);

    // Рисуем цену:
    QString price = Calculator::price(product);
    bool isPrice = isProduct() && PRICE_MAX_CHARS >= price.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    status.price = isPrice ? Calculator::price(product) : NO_DATA;
    emit showControlParam(ControlParam_PriceValue, status.price);
    emit showControlParam(ControlParam_PriceColor, isPrice ? COLOR_ACTIVE : COLOR_PASSIVE);

    // Рисуем стоимость:
    QString amount = Calculator::amount(product, equipmentManager);
    bool isAmount = isWM && isPrice && (isPieceProduct || (isFixed && !isWMError)) &&
        AMOUNT_MAX_CHARS >= amount.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    status.amount = isAmount ? Calculator::amount(product, equipmentManager) : NO_DATA;
    emit showControlParam(ControlParam_AmountValue, status.amount);
    emit showControlParam(ControlParam_AmountColor, isAmount ? COLOR_AMOUNT : COLOR_PASSIVE);

    // Печатать?
    status.isManualPrintEnabled = isAmount && isPM && !equipmentManager->isPMError();
    emit enableManualPrint(status.isManualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && status.isManualPrintEnabled;
    emit showControlParam(ControlParam_PrinterStatus, isWM && equipmentManager->getPMMode() == EquipmentMode_Demo ? "<b>ДЕМО</b>" : "");

    // Внешний дисплей:
    setExternalDisplay();

    // Автопечать
    if(status.isPrintCalculateMode && isPrice)
    {
        if(isWMError || isFixed) status.isPrintCalculateMode = false;
        if(!isPieceProduct) // Весовой товар
        {
            const int wg = (int)(equipmentManager->getWeight() * 1000); // Вес в граммах
            if(!isWMError && isFixed && isAutoPrintEnabled && wg > 0)
            {
                if(wg >= settings->getIntValue(SettingCode_PrintAutoWeight)) print(); // Автопечать
                else showToast("Вес слишком мал для автопечати");
            }
        }
        else // Штучный товар
        {
            const double unitWeight = product[ProductDBTable::UnitWeight].toDouble() / 1000; // Вес единицы, кг
            if(unitWeight > 0) // Задан вес единицы
            {
                if(isFixed && !isWMError)
                {
                    int newPieces = (int)(equipmentManager->getWeight() / unitWeight + 0.5); // Округление до ближайшего целого
                    status.pieces = newPieces < 1 ? 1 : newPieces;
                    if(isAutoPrintEnabled) print(); // Автопечать
                }
            }
            else
            {
                status.isPrintCalculateMode = false;
                if(isAutoPrintEnabled) print(); // Автопечать
            }
        }
    }
    if(isPieceProduct && oldPieces != status.pieces)
    {
        update();
        return;
    }

#ifdef DEBUG_WEIGHT_STATUS
        debugLog(QString("@@@@@ AppManager::updateWeightStatus %1%2%3%4%5 %6%7%8%9%10 %11 %12 %13").arg(
                 Tools::toIntString(isWMError),
                 Tools::toIntString(isAutoPrint),
                 Tools::toIntString(isFixed),
                 Tools::toIntString(isTare),
                 Tools::toIntString(isZero),
                 Tools::toIntString(isWM),
                 Tools::toIntString(isPM),
                 Tools::toIntString(isPieceProduct),
                 Tools::toIntString(status.isManualPrintEnabled),
                 Tools::toIntString(isAutoPrintEnabled),
                 quantity, price, amount));
#endif
}

void AppManager::print() // Печатаем этикетку
{
    debugLog("@@@@@ AppManager::print ");
    QString labelPath;

    // Товар с заданной этикеткой:
    if(isProduct() && Tools::toInt(product[ProductDBTable::LabelFormat].toString()) != 0)
        labelPath = db->getLabelPathById(product[ProductDBTable::Code].toString());

    if(labelPath.isEmpty())
        labelPath = db->getLabelPathByName(settings->getStringValue(SettingCode_PrintLabelFormat));
    equipmentManager->print(db, getCurrentUser(), product, labelPath);
}

void AppManager::setExternalDisplay()
{
    equipmentManager->setExternalDisplay(product);
}

bool AppManager::onClicked(const int clicked)
{
    debugLog(QString("@@@@@ AppManager::onClicked %1").arg(Tools::toString(clicked)));
    bool ok = true;
    onUserAction();

    switch (clicked)
    {
    case Clicked_None:
        break;

    case Clicked_AddUser:
        emit showInputUserPanel("", "", "", false);
        break;

    case Clicked_AdminSettings:
        startSettings();
        break;

    case Clicked_BackgroundDownload:
        if(!BACKGROUND_DOWNLOADING) showAttention("Фоновая загрузка запрещена");
        ok = BACKGROUND_DOWNLOADING;
        break;

    case Clicked_Help:
        showAttention("Не поддерживается");
        break;

    case Clicked_HierarchyUp: // Переход вверх по иерархическому дереву групп товаров
        if (searchPanelModel->hierarchyUp())
        {
            searchPanelModel->descriptor.reset("");
            searchPanelModel->isRoot = false;
            updateSearch();
        }
        break;

    case Clicked_Info:
        setSettingsNetInfo();
        settings->write();
        showMessage(settings->modelInfo(), settings->aboutInfo());
        break;

    case Clicked_Lock:
        showConfirmation(ConfirmSelector_Authorization, "Вы хотите сменить пользователя?", "");
        break;

    case Clicked_Search:
        searchPanelModel->isHierarchy = !searchPanelModel->isHierarchy;
        searchPanelModel->isRoot = searchPanelModel->isHierarchy;
        searchPanelModel->descriptor.reset("");
        updateSearch();
        break;

    case Clicked_Zero:
        equipmentManager->setZero();
        showTareToast(setProductTare());
        update();
        break;

    case Clicked_Tare:
        equipmentManager->setTare();
        showTareToast();
        update();
        break;

    case Clicked_ViewLog:
        db->saveLog(LogType_Info, LogSource_Admin, "Просмотр лога");
        db->select(DBSelector_GetLog);
        emit showViewLogPanel();
        break;

    case Clicked_ShowcaseDirection:
        status.isProductSortIncrement = !status.isProductSortIncrement;
        updateShowcase();
        break;

    case Clicked_SettingsPanelClose:
    {
        emit previousSettings();
        const int groupCode = settings->getCurrentGroupCode();
        if(groupCode != 0)
        {
            DBRecord* r = settings->getByCode(groupCode);
            if(r != nullptr && !r->empty() && Settings::isGroup(*r))
            {
                // Переход вверх:
                updateSettings(r->at(SettingField_GroupCode).toInt());
                emit showSettingsPanel(settings->getCurrentGroupName());
                break;
            }
        }
        stopSettings();
        break;
    }

    case Clicked_ShowcaseAuto:
    {
        switch (status.autoPrintMode)
        {
        case AutoPrintMode_Off:      break;
        case AutoPrintMode_On:       status.autoPrintMode = AutoPrintMode_Disabled; break;
        case AutoPrintMode_Disabled: status.autoPrintMode = AutoPrintMode_On; break;
        }
        showWeightFlags();
        break;
    }

    case Clicked_Print:
        if(status.isManualPrintEnabled)
        {
            status.isPrintCalculateMode = false;
            print();
        }
        break;

    case Clicked_ProductDescription:
    {
        QString s = db->getProductMessage(product);
        if (!s.isEmpty()) showMessage("Описание товара", s);
        break;
    }

    case Clicked_ProductFavorite:
        if(isAdmin() || settings->getBoolValue(SettingCode_ChangeShowcase))
        {
            if(isProductInShowcase(product))
                showConfirmation(ConfirmSelector_RemoveFromShowcase, "Удалить товар из витрины?", "");
            else
                showConfirmation(ConfirmSelector_AddToShowcase, "Добавить товар в витрину?", "");
        }
        break;

    case Clicked_ProductPanelClose:
        resetProduct();
        setMainPage(screenManager->mainPageIndex);
        break;

    case Clicked_ProductPanelPieces:
        if(Calculator::isPiece(product))
        {
            debugLog(QString("@@@@@ AppManager::onProductPanelPiecesClicked %1 %2").arg(
                         Tools::toString(status.pieces),
                         Tools::toString(settings->getIntValue(SettingCode_CharNumberPieces))));
            emit showPiecesInputBox(status.pieces, settings->getIntValue(SettingCode_CharNumberPieces));
        }
        else beepSound();
        break;

    case Clicked_EditUsers:
        users->getAll();
        editUsersPanelModel->update(users);
        emit showEditUsersPanel();
        break;

    case Clicked_Rewind:
        if(isAuthorizationOpened() || isSettingsOpened()) beepSound();
        else equipmentManager->feed();
        break;

    case Clicked_WeightFlags:
        showTareToast();
        break;

    default:
        ok = false;
        break;
    }
    return ok;
}

bool AppManager::onClicked2(const int clicked, const int param)
{
    debugLog(QString("@@@@@ AppManager::onClicked2 %1 %2").arg(Tools::toString(clicked), Tools::toString(param)));
    bool ok = true;
    if(clicked != Clicked_WeightPanel) onUserAction();

    switch (clicked)
    {
    case Clicked_None:
        break;

    case Clicked_WeightPanel:
        if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, [this]() { onUserAction(); } );
        if(param == status.secret + 1 && (++status.secret) == 3) onClicked(Clicked_Lock);
        break;

    case Clicked_ShowcaseSort:
        status.productSort = param;
        switch (status.productSort)
        {
        case ShowcaseSort_Code:
        case ShowcaseSort_Code2: status.lastProductSort = status.productSort; break;
        default: break;
        }
        updateShowcase();
        break;

    case Clicked_Showcase:
        setProduct(showcasePanelModel->productByIndex(param));
        break;

    case Clicked_SearchResult:
        if (param >= 0 && param < searchPanelModel->productCount())
        {
            DBRecord& product = searchPanelModel->productByIndex(param);
            if (!Calculator::isGroup(product)) setProduct(product);
            else if(searchPanelModel->hierarchyDown(product))
            {
                searchPanelModel->descriptor.reset("");
                searchPanelModel->isRoot = false;
                updateSearch();
            }
        }
        break;

    case Clicked_EditUsersPanel:
    {
        DBRecord u = users->getByIndex(param);
        const int code =  Users::getCode(u);
        if(code <= 0)
        {
            showAttention("Редактирование запрещено");
            break;
        }
        const QString name = Users::getName(u);
        const QString password =  Users::getPassword(u);
        const bool isAdmin = Users::isAdmin(u);
        debugLog(QString("@@@@@ AppManager::onClicked2 Clicked_EditUsersPanel %1 %2 %3 %4 %5").arg(
            Tools::toString(param), Tools::toString(code), name, password, Tools::toString(isAdmin)));
        emit showInputUserPanel(Tools::toString(code), name, password, isAdmin);
        break;
    }

    default:
        ok = false;
        break;
    }
    return ok;
}

bool AppManager::onClicked3(const int clicked, const QString &param)
{
    debugLog(QString("@@@@@ AppManager::onClicked3 %1 %2").arg(Tools::toString(clicked), param));
    bool ok = true;

    switch (clicked)
    {
    case Clicked_None:
        break;

    case Clicked_DeleteUser:
        users->onDeleteUser(param);
        break;

    case Clicked_NumberToSearch:
        if(!isAuthorizationOpened() && !isSettingsOpened() && !isProduct())
        {
            QString title = status.lastProductSort == ShowcaseSort_Code2 ? "Поиск по номеру №" : "Поиск по коду #";
            emit showProductCodeInputBox(title, param);
        }
        break;

    case Clicked_SetProductByCode:
    {
        QString s = param;
        if(status.lastProductSort == ShowcaseSort_Code2)
            db->select(DBSelector_SetProductByCode2, s.remove("№"));
        else
        {
            DBRecord r = db->selectByCode(DBTABLENAME_PRODUCTS, s.remove("#"));
            if(r.isEmpty()) showAttention("Товар не найден!");
            else
            {
                emit closeInputProductPanel();
                setProduct(r);
            }
        }
        break;
    }

    default:
        ok = false;
        break;
    }
    return ok;
}

bool AppManager::setProductTare()
{
    if(!isProduct() || !Calculator::hasTare(product)) return false;
    equipmentManager->setTare(Calculator::tare(product));
    return true;
}

