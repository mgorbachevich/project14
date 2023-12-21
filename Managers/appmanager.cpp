#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include "appmanager.h"
#include "resourcedbtable.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "userdbtable.h"
#include "productpanelmodel.h"
#include "tablepanelmodel.h"
#include "usernamemodel.h"
#include "showcasepanelmodel.h"
#include "searchpanelmodel.h"
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

AppManager::AppManager(QQmlContext* qmlContext, const QSize& screenSize, QObject *parent):
    QObject(parent), context(qmlContext)
{
    qDebug() << "@@@@@ AppManager::AppManager";

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
    showcasePanelModel = new ShowcasePanelModel(this);
    tablePanelModel = new TablePanelModel(this);
    settingsPanelModel = new SettingsPanelModel(this);
    searchPanelModel = new SearchPanelModel(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    viewLogPanelModel = new ViewLogPanelModel(this);
    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("tablePanelModel", tablePanelModel);
    context->setContextProperty("settingsPanelModel", settingsPanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);
    context->setContextProperty("viewLogPanelModel", viewLogPanelModel);

    QTimer::singleShot(100, this, &AppManager::start);
    onUserAction();
    timer->start(APP_TIMER_MSEC);

    qDebug() << "@@@@@ AppManager::AppManager Done";
}

void AppManager::onDBStarted()
{
    //showToast("", "Инициализация");
    qDebug() << "@@@@@ AppManager::onDBStarted";
    onUserAction();
    settingsPanelModel->update(settings);
    startAuthorization();
    db->select(DataBase::UpdateSettings, "");
    //showMessage("NetParams", QString("IP = %1").arg(Tools::getNetParams().localHostIP));
    qDebug() << "@@@@@ AppManager::onDBStarted Done";
}

void AppManager::onNetAction(const int action)
{
    qDebug() << "@@@@@ AppManager::onNetAction " << action;
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
    const quint64 now = Tools::currentDateTimeToUInt();

    // Блокировка:
    quint64 waitBlocking = settings.getItemIntValue(SettingCode_Blocking); // минуты
    if(userActionTime > 0 && !isAuthorizationOpened && waitBlocking > 0 && waitBlocking * 1000 * 60 < now - userActionTime)
    {
        qDebug() << "@@@@@ AppManager::onTimer userActionTime";
        userActionTime = 0;
        startAuthorization();
    }

    // Ожидание окончания сетевых запросов:
    if(netActionTime > 0 && netRoutes == 0 && WAIT_NET_ACTION_MSEC < now - netActionTime)
    {
        qDebug() << "@@@@@ AppManager::onTimer netActionTime";
        netActionTime = 0;
        if(isRefreshNeeded)
        {
            qDebug() << "@@@@@ AppManager::onTimer isRefreshNeeded";
            isRefreshNeeded = false;
            db->afterNetAction();
            refreshAll();
            resetProduct();
            emit showMessageBox("ВНИМАНИЕ!", "Товары обновлены!", true);
            /*
            if(!product.isEmpty())
                db->selectByParam(DataBase::RefreshCurrentProduct, product.at(ProductDBTable::Code).toString());
            */
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
    int pp = settings.getItemIntValue(SettingCode_PointPosition);
    return Tools::moneyToText(price(productRecord), pp);
}

QString AppManager::amountAsString(const DBRecord& productRecord)
{
    double q = 0;
    if(ProductDBTable::isPiece(productRecord))
        q = printStatus.pieces;
    else if(!weightManager->isError())
        q = weightManager->getWeight() * (ProductDBTable::is100gBase(productRecord) ? 10 : 1);
    int pp = settings.getItemIntValue(SettingCode_PointPosition);
    return Tools::moneyToText(q * price(productRecord), pp);
}

double AppManager::price(const DBRecord& productRecord)
{
    const int p = ProductDBTable::Price;
    if (productRecord.count() <= p) return 0;
    int pp = settings.getItemIntValue(SettingCode_PointPosition);
    return Tools::priceToDouble(productRecord[p].toString(), pp);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    QString productCode = product[ProductDBTable::Code].toString();
    qDebug() << "@@@@@ AppManager::setProduct " << productCode;
    productPanelModel->update(product, (ProductDBTable*)db->getTable(DBTABLENAME_PRODUCTS));
    emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
    db->saveLog(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
    QString pictureCode = product[ProductDBTable::PictureCode].toString();
    db->select(DataBase::GetImageByResourceCode, pictureCode);
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
    qDebug() << "@@@@@ AppManager::onProductDescriptionClicked";
    onUserAction();
    db->select(DataBase::GetMessageByResourceCode, product[ProductDBTable::MessageCode].toString());
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
    qDebug() << "@@@@@ AppManager::onRewind ";
    onUserAction();
    printManager->feed();
}

void AppManager::filteredSearch()
{
    // Формирование запросов в БД в зависимости от параметров поиска

    QString& v = searchFilterModel->filterValue;
    qDebug() << "@@@@@ AppManager::filteredSearch " << searchFilterModel->index << v;
    switch(searchFilterModel->index)
    {
        case SearchFilterModel::Code:
            db->select(DataBase::GetProductsByFilteredCode, v);
            break;
        case SearchFilterModel::Barcode:
            db->select(DataBase::GetProductsByFilteredBarcode, v);
            break;
         default:
            qDebug() << "@@@@@ AppManager::filteredSearch ERROR";
            //emit saveLog(LogType_Warning, LogSource_AppManager, "Неизвестный фильтр поиска");
            break;
    }
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    // Изменился шаблон поиска

    qDebug() << "@@@@@ AppManager::onSearchFilterEdited " << value;
    searchFilterModel->filterValue = value;
    filteredSearch();
}

void AppManager::onSearchFilterClicked(const int index)
{
    // Изменилось поле поиска (код, штрих-код...)

    qDebug() << "@@@@@ AppManager::onSearchFilterClicked " << index;
    onUserAction();
    searchFilterModel->index = (SearchFilterModel::FilterIndex)index;
    filteredSearch();
}

void AppManager::onTableBackClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    qDebug() << "@@@@@ AppManager::onTableBackClicked";
    onUserAction();
    if (tablePanelModel->groupUp()) updateTablePanel(false);
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    // Настройка изменилась

    qDebug() << "@@@@@ AppManager::onSettingInputClosed " << settingItemCode << value;
    DBRecord* r = settings.getItemByCode(settingItemCode);
    if (r != nullptr && value != (r->at(SettingDBTable::Value)).toString())
    {
        settings.setItemValue(settingItemCode, value);
        db->updateSettingsRecord(DataBase::ReplaceSettingsItem, *r);
        QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(QString::number(settingItemCode), value);
        db->saveLog(LogType_Warning, LogSource_Admin, s);
    }
}

void AppManager::onAdminSettingsClicked()
{
    qDebug() << "@@@@@ AppManager::onAdminSettingsClicked";
    onUserAction();
    db->saveLog(LogType_Info, LogSource_Admin, "Просмотр настроек");
    settingsPanelModel->update(settings);
    emit showSettingsPanel(settings.getCurrentGroupName());
}

void AppManager::beepSound()
{
    Tools::sound(BEEP_SOUND_FILE_PATH);
}

void AppManager::clickSound()
{
    Tools::sound(CLICK_SOUND_FILE_PATH);
}

void AppManager::onLockClicked()
{
    qDebug() << "@@@@@ AppManager::onLockClicked";
    onUserAction();
    emit showConfirmationBox(DialogSelector::Dialog_Authorization, "Подтверждение", "Вы хотите сменить пользователя?");
}

void AppManager::onMainPageChanged(const int index)
{
    qDebug() << "@@@@@ AppManager::onMainPageChanged " << index;
    mainPageIndex = index;
    emit showMainPage(mainPageIndex);
}

void AppManager::onPiecesInputClosed(const QString &value)
{
    qDebug() << "@@@@@ AppManager::onPiecesInputClosed " << value;
    onUserAction();
    int v = Tools::stringToInt(value);
    if(v < 1)
    {
        v = 1;
        emit showMessageBox("ВНИМАНИЕ!", "Количество не должно быть меньше 1", true);
    }
    printStatus.pieces = v;
    updateStatus();
}

void AppManager::onPopupClosed()
{
    if (--openedPopupCount <= 0)
    {
        openedPopupCount = 0;
        emit showMainPage(mainPageIndex);
        isAuthorizationOpened = false;
    }
    qDebug() << "@@@@@ AppManager::onPopupClosed " << openedPopupCount;
}

void AppManager::onPopupOpened()
{
    openedPopupCount++;
    qDebug() << "@@@@@ AppManager::onPopupOpened " << openedPopupCount;
}

void AppManager::onDBRequestResult(const DataBase::Selector selector, const DBRecordList& records, const bool ok)
{
    // Получен результ из БД

    qDebug() << "@@@@@ AppManager::onDBRequestResult " << selector;
    if (!ok)
    {
        emit showMessageBox("ВНИМАНИЕ!", "Ошибка базы данных!", true);
        return;
    }

    switch(selector)
    {
    case DataBase::UpdateSettings: // Обновление настроек с перезапуском оборудования:
        if(!records.isEmpty()) settings.update(records);
        settingsPanelModel->update(settings);
        if(isStarted) startEquipment();
        break;

    case DataBase::ChangeSettings: // Обновление настроек без перезапуска оборудования:
        if(!records.isEmpty()) settings.update(records);
        settingsPanelModel->update(settings);
        //if(started) startEquipment(false, true, true);
        break;

    case DataBase::ReplaceSettingsItem: // Изменена настройка оператором:
        db->select(DataBase::ChangeSettings, "");
        break;

    case DataBase::GetAuthorizationUserByName: // Получен результат поиска пользователя по введеному имени при авторизации:
        checkAuthorization(records);
        break;

    case DataBase::GetShowcaseProducts: // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        db->select(DataBase::GetShowcaseResources, records);
        break;

    case DataBase::GetMessageByResourceCode: // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            emit showMessageBox("Описание товара", records[0][ResourceDBTable::Value].toString(), true);
        break;

    case DataBase::GetUserNames: // Отображение имен пользователей при авторизации:
        showUsers(records);
        break;

    case DataBase::GetLog: // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DataBase::GetProductsByGroupCodeIncludeGroups: // Отображение товаров выбранной группы:
        tablePanelModel->update(records);
        break;

    case DataBase::GetProductsByFilteredCode: // Отображение товаров с заданным фрагментом кода:
        searchPanelModel->update(records, SearchFilterModel::Code);
        break;

    case DataBase::GetProductsByFilteredBarcode: // Отображение товаров с заданным фрагментом штрих-кода:
        searchPanelModel->update(records, SearchFilterModel::Barcode);
        break;

    case DataBase::RefreshCurrentProduct: // Сброс выбранного товара после изменений в БД:
        resetProduct();
        if (!records.isEmpty() && !records.at(0).isEmpty())
        {
            //showToast("ВНИМАНИЕ!", "Выбранный товар изменен");
            setProduct(records.at(0));
        }
        break;

    case DataBase::GetShowcaseResources: // Отображение картинок товаров экрана Showcase:
    {
        qDebug() << "@@@@@ AppManager::onDBRequestResult GetShowcaseResources" << records.count();
        QStringList fileNames;
        for (int i = 0; i < records.count(); i++)
        {
            QString s = getImageFileWithQmlPath(records[i]);
            // qDebug() << "@@@@@ AppManager::onDBRequestResult GetShowcaseResources" << s;
            fileNames.append(s);
        }
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DataBase::GetImageByResourceCode: // Отображение картинки выбранного товара:
    {
        QString imagePath = records.count() > 0 ? getImageFileWithQmlPath(records[0]) : DUMMY_IMAGE_FILE_PATH;
        emit showProductImage(imagePath);
        //showMessage("Image file path", imagePath);
        break;
    }

    default:break;
    }
}

QString AppManager::getImageFileWithQmlPath(const DBRecord& r)
{
    QString path = DUMMY_IMAGE_FILE_PATH;
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
    db->select(DataBase::GetLog, "");
    emit showViewLogPanel();
}

void AppManager::onVirtualKeyboardSet(const int v)
{
    qDebug() << "@@@@@ AppManager::onVirtualKeyboardSet " << v;
    onUserAction();
    emit showVirtualKeyboard(v);
}

void AppManager::onWeightPanelClicked(const int param)
{
    qDebug() << "@@@@@ AppManager::onWeightPanelClicked " << param;
    if(param == 1) QTimer::singleShot(WAIT_SECRET_MSEC, this, &AppManager::onUserAction);
    if(param == secret + 1 && ++secret == 3) onLockClicked();
}

void AppManager::onTareClicked()
{
    qDebug() << "@@@@@ AppManager::onTareClicked ";
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Tare);
    updateStatus();
}

void AppManager::onZeroClicked()
{
    qDebug() << "@@@@@ AppManager::onZeroClicked ";
    onUserAction();
    weightManager->setWeightParam(EquipmentParam_Zero);
    updateStatus();
}

void AppManager::onConfirmationClicked(const int selector)
{
    qDebug() << "@@@@@ AppManager::onConfirmationClicked " << selector;
    onUserAction();
    switch (selector)
    {
    case DialogSelector::Dialog_Authorization:
        startAuthorization();
        break;
    }
}

void AppManager::onInfoClicked()
{
    qDebug() << "@@@@@ AppManager::onInfoClicked ";
    onUserAction();
    emit showMessageBox("Инфо", appInfo.all(EOL), true);
}

void AppManager::onTableResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onTableResultClicked " << index;
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
    qDebug() << "@@@@@ AppManager::onSettingsItemClicked " << index;
    clickSound();
    onUserAction();
    DBRecord* r =  settings.getItemByIndexInCurrentGroup(index);
    if(r == nullptr  || r->empty()) return;

    const int code = (r->at(SettingDBTable::Code)).toInt();
    const int type = (r->at(SettingDBTable::Type)).toInt();
    const QString name = (r->at(SettingDBTable::Name)).toString();
    const QString value = (r->at(SettingDBTable::Value)).toString();

    switch (type)
    {
    case SettingType_Group:
        settings.currentGroupCode = code;
        settingsPanelModel->update(settings);
        emit showSettingsPanel(settings.getCurrentGroupName());
        break;
    case SettingType_ReadOnly:
        emit showMessageBox(name, "Редактирование запрещено", true);
        break;
    case SettingType_InputNumber:
    case SettingType_InputText:
        emit showSettingInputBox(code, name, value);
        break;
    case SettingType_Custom:
        emit showMessageBox(name, "СПЕЦ КОМАНДА", true);
        break;
    case SettingType_IntervalNumber:
        emit showMessageBox(name, "ПОЛЗУНОК", true);
        break;
    case SettingType_List:
        emit showMessageBox(name, "ВЫБОР ИЗ СПИСКА", true);
        break;
    default:
        break;
    }
}

void AppManager::onSettingsPanelCloseClicked()
{
    qDebug() << "@@@@@ AppManager::onSettingsPanelCloseClicked " << settings.currentGroupCode;
    onUserAction();
    emit closeSettings();
    if(settings.currentGroupCode != 0)
    {
        DBRecord* r =  settings.getItemByCode(settings.currentGroupCode);
        if(r != nullptr && !r->empty() && settings.isGroup(*r))
        {
            settings.currentGroupCode = r->at(SettingDBTable::GroupCode).toInt();
            settingsPanelModel->update(settings);
            emit showSettingsPanel(settings.getCurrentGroupName());
        }
    }
}

void AppManager::onSearchResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSearchResultClicked " << index;
    onUserAction();
    setProduct(searchPanelModel->productByIndex(index));
}

void AppManager::onShowcaseClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onShowcaseClicked " << index;
    onUserAction();
    setProduct(showcasePanelModel->productByIndex(index));
}

void AppManager::onSwipeMainPage(const int page)
{
    qDebug() << "@@@@@ AppManager::onSwipeMainPage " << page;
    emit showMainPage(page);
}

void AppManager::updateTablePanel(const bool root)
{
    qDebug() << "@@@@@ AppManager::updateTablePanel";
    if (root) tablePanelModel->root();
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    db->select(DataBase::GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::startAuthorization()
{
    qDebug() << "@@@@@ AppManager::startAuthorization";
    isStarted = false;
    stopEquipment();
    QString info = appInfo.all();
    qDebug() << "@@@@@ AppManager::startAuthorization " << info;
    emit showAuthorizationPanel(info);
    isAuthorizationOpened = true;
    db->saveLog(LogType_Warning, LogSource_User, "Авторизация");
    db->select(DataBase::GetUserNames, "");
    qDebug() << "@@@@@ AppManager::startAuthorization Done";
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    onUserAction();
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    qDebug() << "@@@@@ AppManager::onCheckAuthorizationClick " << normalizedLogin << password;
    user[UserDBTable::Name] = normalizedLogin;
    user[UserDBTable::Password] = password;
    db->select(DataBase::GetAuthorizationUserByName, normalizedLogin);
}

void AppManager::checkAuthorization(const DBRecordList& dbUsers)
{
    // Введены логин и пароль. Проверка

    qDebug() << "@@@@@ AppManager::checkAuthorization";
    QString title = "Авторизация";
    QString error = "";
    QString login = user[UserDBTable::Name].toString();
    QString password = user[UserDBTable::Password].toString();
    bool isDefaultAdmin = dbUsers.isEmpty();

    if(CHECK_AUTHORIZATION)
    {
        if (!isDefaultAdmin)
        {
            if (login != dbUsers[0][UserDBTable::Name].toString() || password != dbUsers[0][UserDBTable::Password])
            {
                error = "Неверные имя пользователя или пароль";
                qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
                emit showMessageBox(title, error, true);
                db->saveLog(LogType_Warning, LogSource_User, QString("%1. %2").arg(title, error));
                return;
            }
            user.clear();
            user.append(dbUsers[0]);
        }
    }
    else
    {
        user = UserDBTable::defaultAdmin();
        login = user[UserDBTable::Name].toString();
    }

    qDebug() << "@@@@@ AppManager::checkAuthorization OK";
    QString s = QString("%1. Пользователь: %2. Код: %3").arg(title, login, user[UserDBTable::Code].toString());
    db->saveLog(LogType_Warning, LogSource_User, s);
    if(!isStarted)
    {
        isStarted = true;
        startEquipment();
        // showMessage(title, "Успешно!");
        emit authorizationSucceded();
        refreshAll();
        resetProduct();
        mainPageIndex = 0;
        emit showMainPage(mainPageIndex);
        isAuthorizationOpened = false;
    }
}

void AppManager::refreshAll()
{
    // Обновить всё на экране

    qDebug() << "@@@@@ AppManager::refreshAll";
    emit showAdminMenu(UserDBTable::isAdmin(user));
    db->select(DataBase::GetShowcaseProducts, "");
    searchFilterModel->update();
    //searchPanelModel->update();
    filteredSearch();
    updateTablePanel(true);
}

void AppManager::showToast(const QString &title, const QString &text, const int delaySec)
{
    emit showMessageBox(title, text, false);
    QTimer::singleShot(delaySec * 1000, this, &AppManager::hideToast);
}

void AppManager::resetProduct()
{
    // Сбросить выбранный продукт
    qDebug() << "@@@@@ AppManager::resetProduct";
    product.clear();
    printStatus.onResetProduct();
    emit resetCurrentProduct();
    updateStatus();
}

void AppManager::startEquipment(const bool server, const bool weight, const bool printer)
{
    qDebug() << "@@@@@ AppManager::startEquipment";
    if(server) netServer->start(settings.getItemIntValue(SettingCode_TCPPort));

    QString demoMessage = "", url1, url2;
    int e1 = 0, e2 = 0;

    if(weight)
    {
        if(WM_DEMO)
        {
            url1 = "demo://COM3?baudrate=115200&timeout=100";
            demoMessage += "\nДемо-режим весового модуля";
        }
        else if(WM_HTTP)
        {
            url1 = "http://127.0.0.1:51233";
        }
        else
        {
            /*
            QString address = settings.getItemStringValue(SettingCode_WMAddress);
            QString boudrate =  QString::number(Settings::getBoudrate(settings.getItemIntValue(SettingCode_WMBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingCode_WMTimeout));
            url1 = QString("serial://%1?baudrate=%2&timeout=%3").arg(address, boudrate, timeout);
            */
        }
        e1 = weightManager->start(url1);
    }

    if(printer)
    {
        if(PRINTER_DEMO)
        {
            url2 = "demo://COM3?baudrate=115200&timeout=100";
            demoMessage += "\nДемо-режим принтера";
        }
        else if(PRINTER_HTTP)
        {
            url2 = "http://127.0.0.1:51232";
        }
        else
        {
            /*
            QString address = settings.getItemStringValue(SettingCode_PrinterAddress);
            QString boudrate =  QString::number(Settings::getBoudrate(settings.getItemIntValue(SettingCode_PrinterBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingCode_PrinterTimeout));
            url2 = QString("serial://%1?baudrate=%2&timeout=%3").arg(address, boudrate, timeout);
            */
        }
        e2 = printManager->start(url2);
    }

    if(e1 != 0) emit showMessageBox("ВНИМАНИЕ!", QString("Ошибка весового модуля %1!\n%2").
                                    arg(QString::number(e1), weightManager->getErrorDescription(e1)), true);
    if(e2 != 0) emit showMessageBox("ВНИМАНИЕ!", QString("Ошибка принтера %1!\n%2").
                                    arg(QString::number(e2), printManager->getErrorDescription(e2)), true);
    if(!demoMessage.isEmpty()) emit showMessageBox("ВНИМАНИЕ!", demoMessage, true);
}

void AppManager::stopEquipment()
{
    netServer->stop();
    weightManager->stop();
    printManager->stop();
}

void AppManager::onUserAction()
{
    qDebug() << "@@@@@ AppManager::onUserAction";
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
        return;
    }
    for (int i = 0; i < users.count(); i++)
    {
        if(users.at(i).at(UserDBTable::Code).toInt() == user.at(UserDBTable::Code).toInt())
        {
            emit setCurrentUser(i, users.at(i).at(UserDBTable::Name).toString());
            return;
        }
    }
}

void AppManager::print() // Печатаем этикетку
{
    qDebug() << "@@@@@ AppManager::print ";
    printManager->print(user, product, quantityAsString(product), priceAsString(product), amountAsString(product));
}

void AppManager::onPrintClicked()
{
    qDebug() << "@@@@@ AppManager::onPrintClicked ";
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

    qDebug() << "@@@@@ AppManager::onPrinted";
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
    qDebug() << QString("@@@@@ AppManager::onEquipmentParamChanged %1 %2").arg(param, errorCode);

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

    qDebug() << QString("@@@@@ AppManager::updateStatus %1 %2 %3 %4 %5 %6 %7").arg(
                    Tools::boolToString(isWeightError),
                    Tools::boolToString(isAutoPrint),
                    Tools::boolToString(isTare),
                    Tools::boolToString(isZero),
                    quantity, price, amount);
}











