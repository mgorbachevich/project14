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

#ifdef CREATE_DEFAULT_DATA_ON_START
    if(!Tools::isFileExists(DB_PRODUCT_NAME))
    {
        DataBase::removeDBFile(DB_PRODUCT_NAME);
        DataBase::removeDBFile(DB_LOG_NAME);
        DataBase::removeDBFile(DEBUG_LOG_NAME);
        Tools::copyFile(QString(":/Default/%1").arg(DB_PRODUCT_NAME), Tools::dbPath(DB_PRODUCT_NAME));
    }
    if(!Tools::isFileExists(Tools::dbPath(QString("%1/pictures/%2").arg(DOWNLOAD_SUBDIR, "1.png"))))
    {
        QStringList images = { "1.png", "2.png", "3.jpg", "4.png", "5.png", "6.png", "8.png",
                               "9.png", "10.png", "11.png", "12.png", "15.png" };
        for (int i = 0; i < images.count(); i++)
            Tools::copyFile(QString(":/Default/%1").arg(images[i]),
                            Tools::dbPath(QString("%1/pictures/%2").arg(DOWNLOAD_SUBDIR, images[i])));
    }
#endif

    settings = new Settings(this);
    users = new Users(this);
    showcase = new Showcase(this);
    db = new DataBase(this);
    netServer = new NetServer(this);
    equipmentManager = new EquipmentManager(this);
    moneyCalculator = new MoneyCalculator(this);
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

void AppManager::onDBStarted()
{
    debugLog("@@@@@ AppManager::onDBStarted");
    onUserAction();
    setSettingsInfo();
    setSettingsNetInfo();
    setSettingsLabels();
    settings->write();

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
    }
    else if(isSettingsOpened()) // Настройки
    {
        status.isAlarm = false;
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
        else updateWeightStatus();
    }

    // Ожидание окончания сетевых запросов:
    if(netServer->isStarted() && status.netActionTime > 0 &&
            WAIT_NET_COMMAND_MSEC < now - status.netActionTime)
    {
        debugLog("@@@@@ AppManager::onTimer netActionTime");
        onNetCommand(NetCommand_StopLoad, Tools::toString(false));
    }
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
        if(db->isProductInShowcase(product))
            showConfirmation(ConfirmSelector_RemoveFromShowcase,  "Удалить товар из витрины?", "");
        else
            showConfirmation(ConfirmSelector_AddToShowcase, "Добавить товар в витрину?", "");
    }
}

void AppManager::onProductPanelCloseClicked()
{
    onUserAction();
    resetProduct();
    setMainPage(screenManager->mainPageIndex);
}

void AppManager::onProductPanelPiecesClicked()
{
    onUserAction();
    if(ProductDBTable::isPiece(product))
    {
        debugLog(QString("@@@@@ AppManager::onProductPanelPiecesClicked %1 %2").arg(
                     Tools::toString(status.pieces),
                     Tools::toString(settings->getIntValue(SettingCode_CharNumberPieces))));
        emit showPiecesInputBox(status.pieces, settings->getIntValue(SettingCode_CharNumberPieces));
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
    showConfirmation(ConfirmSelector::ConfirmSelector_Authorization, "Вы хотите сменить пользователя?", "");
}

void AppManager::onNumberToSearchClicked(const QString &s)
{
    if(!isAuthorizationOpened() && !isSettingsOpened() && !isProduct())
    {
        debugLog("@@@@@ AppManager::onNumberToSearchClicked " + s);
        QString title = status.lastProductSort == ShowcaseSort_Code2 ? "Поиск по номеру №" : "Поиск по коду #";
        emit showProductCodeInputBox(title, s);
    }
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
    updateWeightStatus();
}

void AppManager::onSetProductByCodeClicked(const QString &value)
{
    debugLog("@@@@@ AppManager::onSetProductByCodeClicked " + value);
    QString s = value;
    if(status.lastProductSort == ShowcaseSort_Code2)
        db->select(DBSelector_SetProductByCode2, s.remove("№"));
    else
        db->select(DBSelector_SetProductByCode, s.remove("#"));
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
    case DBSelector_GetMessageByResourceCode: // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            showMessage("Описание товара", records[0][ResourceDBTable::Value].toString());
        break;

    case DBSelector_GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DBSelector_GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        //showFoundProductsToast(records.count());
        emit showHierarchyRoot(searchPanelModel->isHierarchyRoot());
        emit showControlParam(ControlParam_SearchTitle, searchPanelModel->hierarchyTitle());
        searchPanelModel->update(records, -1);
        break;

    case DBSelector_SetProductByCode: // Отображение товара с кодом:
        if(records.isEmpty())
        {
            showAttention("Товар не найден!");
            break;
        }
        emit closeInputProductPanel();
        setProduct(records.at(0));
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

    case DBSelector_GetProductByCode: // Отображение товара с кодом:
        emit enableSetProductByInputCode(records.count() == 1 && !records.at(0).isEmpty());
        break;

    case DBSelector_GetProductsByInputCode: // Отображение товаров с фрагментом кода:
        showFoundProductsToast(records.count());
        inputProductCodePanelModel->update(records);
        db->select(DBSelector_GetProductByCode, inputProductCodePanelModel->descriptor.param); // Enable/disable button "Continue"
        break;

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

   case DBSelector_RefreshCurrentProduct: // Сброс выбранного товара после изменений в БД:
        resetProduct();
        if (!records.isEmpty() && !records.at(0).isEmpty())
        {
            //showToast("ВНИМАНИЕ! Выбранный товар изменен");
            setProduct(records.at(0));
        }
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

    case DBSelector_GetImageByResourceCode: // Отображение картинки выбранного товара:
    {
        QString imagePath = records.count() > 0 ? getImageFileWithQmlPath(records[0]) : DUMMY_IMAGE_FILE;
        emit showControlParam(ControlParam_ProductImage, imagePath);
        debugLog("@@@@@ AppManager::onSelectResult showProductImage " + imagePath);
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
    debugLog("@@@@@ AppManager::onViewLogClicked ");
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр лога");
    onUserAction();
    db->select(DBSelector_GetLog);
    emit showViewLogPanel();
}

void AppManager::onVirtualKeyboardSet(const int v)
{
    debugLog("@@@@@ AppManager::onVirtualKeyboardSet " + Tools::toString(v));
    onUserAction();
    emit showVirtualKeyboard(v);
}

void AppManager::onWeightPanelClicked(const int param)
{
    debugLog("@@@@@ AppManager::onWeightPanelClicked " + Tools::toString(param));
    if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, [this]() { onUserAction(); } );
    if(param == status.secret + 1 && (++status.secret) == 3) onLockClicked();
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
                QString s = QDateTime(d, t).toString(DATE_TIME_FORMAT);
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
        onSettingInputClosed(SettingCode_DateTime, param);
        equipmentManager->setSystemDateTime(QDateTime::fromString(param, DATE_TIME_FORMAT));
        break;

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
        db->removeProductFromShowcase(product);
        emit setCurrentProductFavorite(db->isProductInShowcase(product));
        updateShowcase();
        break;

    case ConfirmSelector_AddToShowcase:
        db->addProductToShowcase(product);
        emit setCurrentProductFavorite(db->isProductInShowcase(product));
        updateShowcase();
        break;
    }
}

void AppManager::onDeleteUserClicked(const QString& code)
{
    debugLog("@@@@@ AppManager::onDeleteUserClicked " + code);
    users->onDeleteUser(code);
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
    debugLog(QString("@@@@@ AppManager::onSettingsItemClicked %1 %2 %3").arg(Tools::toString(code), name, QString::number(type)));

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
        onEditUsersClicked();
        return;

    case SettingCode_DateTime:
    case SettingCode_VerificationDate:
        showDateInputPanel(code);
        return;

    case SettingCode_Group_Info:
        setSettingsNetInfo();
        settings->write();
        break;

    default:
        break;
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
        showSettingComboBox(*r);
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

    default:
        break;
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

void AppManager::onSettingsPanelCloseClicked()
{
    debugLog("@@@@@ AppManager::onSettingsPanelCloseClicked " + Tools::toString(settings->getCurrentGroupCode()));
    onUserAction();
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
            return;
        }
    }
    stopSettings();
}

void AppManager::onShowcaseAutoClicked()
{
    switch (status.autoPrintMode)
    {
    case AutoPrintMode_Off:      return;
    case AutoPrintMode_On:       status.autoPrintMode = AutoPrintMode_Disabled; break;
    case AutoPrintMode_Disabled: status.autoPrintMode = AutoPrintMode_On; break;
    }
    emit showControlParam(ControlParam_AutoPrint, Tools::toString((int)status.autoPrintMode));
}

void AppManager::onShowcaseClicked(const int index)
{
    debugLog("@@@@@ AppManager::onShowcaseClicked " + Tools::toString(index));
    onUserAction();
    setProduct(showcasePanelModel->productByIndex(index));
}

void AppManager::onShowcaseDirectionClicked()
{
    onUserAction();
    status.isProductSortIncrement = !status.isProductSortIncrement;
    updateShowcase();
}

void AppManager::onShowcaseSortClicked(const int sort)
{
    debugLog("@@@@@ AppManager::onShowcaseSortClicked " + Tools::toString(status.productSort));
    onUserAction();
    status.productSort = sort;
    switch (status.productSort)
    {
    case ShowcaseSort_Code:
    case ShowcaseSort_Code2: status.lastProductSort = status.productSort; break;
    default: break;
    }
    updateShowcase();
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
        debugLog("@@@@@ AppManager::startAuthorization Done");
    });
}

void AppManager::updateShowcase()
{
    showWait(true);
    showcase->getAll();
    emit showControlParam(ControlParam_AutoPrint, Tools::toString((int)status.autoPrintMode));
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
    return db->netUpload(t, s, b);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    if(!product.isEmpty())
    {
        QString productCode = product[ProductDBTable::Code].toString();
        debugLog("@@@@@ AppManager::setProduct " + productCode);
        productPanelModel->update(product, moneyCalculator->priceAsString(product), (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
        emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
        db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
        QString pictureCode = product[ProductDBTable::PictureCode].toString();
        db->select(DBSelector_GetImageByResourceCode, pictureCode);
        emit setCurrentProductFavorite(db->isProductInShowcase(product));
        updateWeightStatus();
    }
}

void AppManager::showFoundProductsToast(const int v)
{
    // if(v > 1) showToast(QString("Найдено товаров: %1").arg(Tools::toString(v)));
}

void AppManager::resetProduct() // Сбросить выбранный продукт
{
    isResetProductNeeded = false;
    if(isProduct())
    {
        debugLog("@@@@@ AppManager::resetProduct");
        product.clear();
        status.onResetProduct();
        emit resetCurrentProduct();
    }
    updateWeightStatus();
}

void AppManager::onUserAction()
{
    debugLog("@@@@@ AppManager::onUserAction");
#ifdef DEBUG_MEMORY_MESSAGE
    Tools::debugMemory();
#endif
    status.onUserAction();
}

void AppManager::print() // Печатаем этикетку
{
    debugLog("@@@@@ AppManager::print ");
    QString labelPath;

    // Товар с заданной этикеткой:
    if(Tools::toInt(product[ProductDBTable::LabelFormat].toString()) != 0)
        labelPath = db->getLabelPathById(product[ProductDBTable::Code].toString());

    if(labelPath.isEmpty())
        labelPath = db->getLabelPathByName(settings->getStringValue(SettingCode_PrintLabelFormat));
    equipmentManager->print(db,
                            getCurrentUser(),
                            product,
                            moneyCalculator->quantityAsString(product),
                            moneyCalculator->priceAsString(product),
                            moneyCalculator->amountAsString(product),
                            labelPath);
}

void AppManager::onPrintClicked()
{
    debugLog("@@@@@ AppManager::onPrintClicked ");
    onUserAction();
    if(status.isManualPrintEnabled)
    {
        status.isPrintCalculateMode = false;
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
#ifdef DEBUG_WEIGHT_STATUS
    debugLog(QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(
                 Tools::toString(param), Tools::toString(errorCode)));
#endif

    switch (param)
    {
    case ControlParam_None: return;

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

void AppManager::updateWeightStatus()
{
#ifdef DEBUG_WEIGHT_STATUS
    debugLog("@@@@@ AppManager::updateWeightStatus");
#endif
    const bool isPieceProduct = isProduct() && ProductDBTable::isPiece(product);
    const int oldPieces = status.pieces;
    if(isPieceProduct && status.pieces < 1) status.pieces = 1;

    const bool isWM = equipmentManager->isWM();
    const bool isPM = equipmentManager->isPM();
    const bool isZero = equipmentManager->isZeroFlag();
    const bool isTare = equipmentManager->isTareFlag();
    const bool isWMError = equipmentManager->isWMError() || !isWM;
    const bool isFixed = equipmentManager->isWeightFixed();

    const bool isAutoPrint = status.autoPrintMode == AutoPrintMode_On &&
            (!isPieceProduct || settings->getBoolValue(SettingCode_PrintAutoPcs));
    const QString passiveColor = "#424242";
    const QString activeColor = "#fafafa";
    const QString amountColor = "#ffe0b2";

    // Проверка флага 0 - новый товар на весах (начинать обязательно с этого!):
    if(isZero) status.isPrintCalculateMode = true;

    // Рисуем флажки:
    emit showControlParam(ControlParam_Zero, Tools::toString(isZero));
    emit showControlParam(ControlParam_Tare, Tools::toString(isTare));
    emit showControlParam(ControlParam_WeightFixed, Tools::toString(isFixed));
    if(isWMError)
        emit showControlParam(ControlParam_WeightError, Tools::toString(isWMError));
    else
        emit showControlParam(ControlParam_AutoPrint, Tools::toString((int)status.autoPrintMode));

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
            if(isPieceProduct|| !isWMError)
                quantity = moneyCalculator->quantityAsString(product);
        }
    }
    emit showControlParam(ControlParam_WeightValue, quantity);
    emit showControlParam(ControlParam_WeightColor, isPieceProduct || (isFixed && !isWMError) ? activeColor : passiveColor);

    // Рисуем цену:
    QString ps = moneyCalculator->priceAsString(product);
    bool isPrice = isProduct() && PRICE_MAX_CHARS >= ps.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString price = isPrice ? moneyCalculator->priceAsString(product) : NO_DATA;
    emit showControlParam(ControlParam_PriceValue, price);
    emit showControlParam(ControlParam_PriceColor, isPrice ? activeColor : passiveColor);

    // Рисуем стоимость:
    QString as = moneyCalculator->amountAsString(product);
    bool isAmount = isWM && isPrice && (isPieceProduct || (isFixed && !isWMError)) &&
        AMOUNT_MAX_CHARS >= as.replace(QString("."), QString("")).replace(QString(","), QString("")).length();
    QString amount = isAmount ? moneyCalculator->amountAsString(product) : NO_DATA;
    emit showControlParam(ControlParam_AmountValue, amount);
    emit showControlParam(ControlParam_AmountColor, isAmount ? amountColor : passiveColor);

    // Печатать?
    status.isManualPrintEnabled = isAmount && isPM && !equipmentManager->isPMError();
    emit enableManualPrint(status.isManualPrintEnabled);
    const bool isAutoPrintEnabled = isAutoPrint && status.isManualPrintEnabled;
    emit showControlParam(ControlParam_PrinterStatus, isWM && equipmentManager->isPMDemoMode() ? "<b>ДЕМО</b>" : "");

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
        updateWeightStatus();
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

void AppManager::stopAll()
{
    debugLog("@@@@@ AppManager::stopAll");
    status.onStopAll();
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

void AppManager::onEditUsersClicked()
{
    users->getAll();
    debugLog("@@@@@ AppManager::onEditUsersClicked " + Tools::toString(users->count()));
    editUsersPanelModel->update(users);
    emit showEditUsersPanel();
}

void AppManager::onEditUsersPanelClicked(const int index)
{
    DBRecord u = users->getByIndex(index);
    const int code =  Users::getCode(u);
    if(code <= 0)
    {
        showAttention("Редактирование запрещено");
        return;
    }
    const QString name = Users::getName(u);
    const QString password =  Users::getPassword(u);
    const bool isAdmin = Users::isAdmin(u);
    debugLog(QString("@@@@@ AppManager::onEditUsersPanelClicked %1 %2 %3 %4 %5").arg(
        Tools::toString(index), Tools::toString(code), name, password, Tools::toString(isAdmin)));
    emit showInputUserPanel(Tools::toString(code), name, password, isAdmin);
}

void AppManager::onEditUsersPanelClose()
{
    debugLog("@@@@@ AppManager::onEditUsersPanelClose ");
    users->clear();
}

void AppManager::onHelpClicked()
{
    showAttention("Не поддерживается");
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
        debugLog("@@@@@ AppManager::stopAuthorization Done");
    });
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

    default:
        break;
    }
}

bool AppManager::onBackgroundDownloadClicked()
{
    if(!BACKGROUND_DOWNLOADING) showAttention("Фоновая загрузка запрещена");
    return BACKGROUND_DOWNLOADING;
}

void AppManager::onSearchClicked()
{
    debugLog("@@@@@ AppManager::onSearchClicked ");
    onUserAction();
    searchPanelModel->isHierarchy = !searchPanelModel->isHierarchy;
    searchPanelModel->isRoot = searchPanelModel->isHierarchy;
    searchPanelModel->descriptor.reset("");
    updateSearch();
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

void AppManager::onSearchResultClicked(const int index)
{
    debugLog("@@@@@ AppManager::onSearchResultClicked " + Tools::toString(index));
    onUserAction();
    if (index >= 0 && index < searchPanelModel->productCount())
    {
        DBRecord& clicked = searchPanelModel->productByIndex(index);
        if (!ProductDBTable::isGroup(clicked)) setProduct(clicked);
        else if(searchPanelModel->hierarchyDown(clicked))
        {
            searchPanelModel->descriptor.reset("");
            searchPanelModel->isRoot = false;
            updateSearch();
        }
    }
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

void AppManager::onHierarchyUpClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    debugLog("@@@@@ AppManager::onHierarchyUpClicked");
    onUserAction();
    if (searchPanelModel->hierarchyUp())
    {
        searchPanelModel->descriptor.reset("");
        searchPanelModel->isRoot = false;
        updateSearch();
    }
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
                status.downloadDateTime = Tools::now().toString(DATE_TIME_FORMAT);
                QString s = status.downloadDateTime;
                if(status.downloadedRecords > 0)
                    s += QString(" (Записи %1)").arg(Tools::toString(status.downloadedRecords));
                settings->setValue(SettingCode_InfoLastDownload, s);
                setSettingsLabels();
                settings->write();
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
                else toast = "ОШИБКИ ПРИ ЗАГРУЗКЕ! ВОЗМОЖНА ПОТЕРЯ ДАННЫХ! ДАННЫЕ ОБНОВЛЕНЫ!";
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

void AppManager::onInfoClicked()
{
    debugLog("@@@@@ AppManager::onInfoClicked ");
    onUserAction();
    setSettingsNetInfo();
    settings->write();
    showMessage(settings->modelInfo(), settings->aboutInfo());
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

void AppManager::showSettingComboBox(const DBRecord& r)
{
    emit showSettingComboBox(Settings::getCode(r),
                             Settings::getName(r),
                             settings->getIntValue(r, true),
                             settings->getStringValue(r),
                             settings->getComment(r));
}

void AppManager::setSettingsLabels()
{
    debugLog("@@@@@ AppManager::setSettingsLabels ");
    db->select(DBSelector_GetAllLabels);
}
