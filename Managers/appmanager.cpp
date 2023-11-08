#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include "appmanager.h"
#include "settinggroupdbtable.h"
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
#include "settinggroupspanelmodel.h"
#include "searchfiltermodel.h"
#include "dbthread.h"
#include "tools.h"
#include "appinfo.h"
#include "weightmanager.h"
#include "printmanager.h"
#include "netserver.h"

AppManager::AppManager(QQmlContext* context, QObject *parent, const QSize& screenSize): QObject(parent)
{
    qDebug() << "@@@@@ AppManager::AppManager " << screenSize.width() << screenSize.height();
    this->context = context;

    // Размер экрана и масштабирование:
    this->screenSize = screenSize;
    double hk = ((double)screenSize.height()) / DEFAULT_SCREEN_HEIGHT;
    double wk = ((double)screenSize.width()) / DEFAULT_SCREEN_WIDTH;
    screenScale = wk < hk ? wk : hk;

    user = UserDBTable::defaultAdmin();
    db = new DataBase(DB_FILENAME, settings, this);
    dbThread = new DBThread(db, this);
    netServer = new NetServer(this, db);
    weightManager = new WeightManager(this, WM_DEMO);
    printManager = new PrintManager(this, db, settings, PRINTER_DEMO);
    QTimer *timer = new QTimer(this);

    appInfo.appVersion = APP_VERSION;
    appInfo.dbVersion = db->version();
    appInfo.weightManagerVersion = weightManager->version();
    appInfo.printManagerVersion = printManager->version();
    appInfo.netServerVersion = netServer->version();
    appInfo.ip = Tools::getNetParams().localHostIP;

    connect(timer, &QTimer::timeout, this, &AppManager::onTimer);
    connect(this, &AppManager::start, db, &DataBase::onStart);
    connect(this, &AppManager::transaction, db, &DataBase::onTransaction);
    connect(this, &AppManager::log, db, &DataBase::onLog);
    connect(this, &AppManager::selectFromDB, db, &DataBase::onSelect);
    connect(this, &AppManager::selectFromDBByList, db, &DataBase::onSelectByList);
    connect(this, &AppManager::updateDBRecord, db, &DataBase::onUpdateRecord);
    connect(db, &DataBase::started, this, &AppManager::onDBStarted);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(db, &DataBase::updateDBFinished, this, &AppManager::onUpdateDBFinished);
    connect(weightManager, &WeightManager::paramChanged, this, &AppManager::onEquipmentParamChanged);
    connect(printManager, &PrintManager::printed, this, &AppManager::onPrinted);
    connect(printManager, &PrintManager::paramChanged, this, &AppManager::onEquipmentParamChanged);

    productPanelModel = new ProductPanelModel(this);
    showcasePanelModel = new ShowcasePanelModel(this);
    tablePanelModel = new TablePanelModel(this);
    settingsPanelModel = new SettingsPanelModel(this);
    settingGroupsPanelModel = new SettingGroupsPanelModel(this);
    searchPanelModel = new SearchPanelModel(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    viewLogPanelModel = new ViewLogPanelModel(this);
    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("tablePanelModel", tablePanelModel);
    context->setContextProperty("settingsPanelModel", settingsPanelModel);
    context->setContextProperty("settingGroupsPanelModel", settingGroupsPanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);
    context->setContextProperty("viewLogPanelModel", viewLogPanelModel);

    dbThread->start();
    QTimer::singleShot(100, this, &AppManager::start);
    onUserAction();
    timer->start(APP_TIMER_MSEC);
}

AppManager::~AppManager()
{
    dbThread->stop();
}

void AppManager::onDBStarted()
{
    //showToast("", "Инициализация");
    qDebug() << "@@@@@ AppManager::onDBStarted";
    onUserAction();
    settings.createGroups((SettingGroupDBTable*)db->getTableByName(DBTABLENAME_SETTINGGROUPS));
    settingGroupsPanelModel->update(settings);
    startAuthorization();
    emit selectFromDB(DataBase::GetSettings, "");
    //showMessage("NetParams", QString("IP = %1").arg(Tools::getNetParams().localHostIP));
}

void AppManager::onUpdateDBFinished(const QString& comment)
{
    qDebug() << "@@@@@ AppManager::onUpdateDBFinished " << comment;
    refreshAll();
    if(!product.isEmpty())
        emit selectFromDB(DataBase::RefreshCurrentProduct, product.at(ProductDBTable::Code).toString());
}

QString AppManager::quantityAsString(const DBRecord& productRecord)
{
    if(ProductDBTable::isPiece(productRecord)) return QString("%1").arg(printStatus.pieces);
    return weightManager->isError() ? "" : QString("%1").arg(weightManager->getWeight(), 0, 'f', 3);
}

QString AppManager::priceAsString(const DBRecord& productRecord)
{
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::moneyToText(price(productRecord), pp);
}

QString AppManager::amountAsString(const DBRecord& productRecord)
{
    double q = 0;
    if(ProductDBTable::isPiece(productRecord))
        q = printStatus.pieces;
    else if(!weightManager->isError())
        q = weightManager->getWeight() * (ProductDBTable::is100gBase(productRecord) ? 10 : 1);
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::moneyToText(q * price(productRecord), pp);
}

double AppManager::price(const DBRecord& productRecord)
{
    const int p = ProductDBTable::Price;
    if (productRecord.count() <= p)
        return 0;
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::priceToDouble(productRecord[p].toString(), pp);
}

void AppManager::setProduct(const DBRecord& newProduct)
{
    product = newProduct;
    QString productCode = product[ProductDBTable::Code].toString();
    qDebug() << "@@@@@ AppManager::setProduct " << productCode;
    productPanelModel->update(product, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    emit showProductPanel(product[ProductDBTable::Name].toString(), ProductDBTable::isPiece(product));
    emit log(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
    QString pictureCode = product[ProductDBTable::PictureCode].toString();
    emit selectFromDB(DataBase::GetImageByResourceCode, pictureCode);
    printStatus.onNewProduct();
    updateStatus();

    if (settings.getItemIntValue(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Time)
    {
        int resetTime = settings.getItemIntValue(SettingDBTable::SettingCode_ProductResetTime);
        if (resetTime > 0) QTimer::singleShot(resetTime * 1000, this, &AppManager::resetProduct);
    }
}

void AppManager::onProductDescriptionClicked()
{
    qDebug() << "@@@@@ AppManager::onProductDescriptionClicked";
    emit selectFromDB(DataBase::GetMessageByResourceCode, product[ProductDBTable::MessageCode].toString());
}

void AppManager::onProductPanelCloseClicked()
{
    resetProduct();
}

void AppManager::onProductPanelPiecesClicked()
{
    if(ProductDBTable::isPiece(product)) emit showPiecesInputBox(printStatus.pieces);
    else beep();
}

void AppManager::onRewind() // Перемотка
{
    qDebug() << "@@@@@ AppManager::onRewind ";
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
            emit selectFromDB(DataBase::GetProductsByFilteredCode, v);
            break;
        case SearchFilterModel::Barcode:
            emit selectFromDB(DataBase::GetProductsByFilteredBarcode, v);
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

void AppManager::onSearchOptionsClicked()
{
    emit showSearchOptions();
}

void AppManager::onSearchFilterClicked(const int index)
{
    // Изменилось поле поиска (код, штрих-код...)

    qDebug() << "@@@@@ AppManager::onSearchFilterClicked " << index;
    searchFilterModel->index = (SearchFilterModel::FilterIndex)index;
    filteredSearch();
}

void AppManager::onTableBackClicked()
{
    // Переход вверх по иерархическому дереву групп товаров

    qDebug() << "@@@@@ AppManager::onTableBackClicked";
    if (tablePanelModel->groupUp()) updateTablePanel(false);
}

void AppManager::onTableOptionsClicked()
{
    emit showTableOptions();
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    // Настройка изменилась

    qDebug() << "@@@@@ AppManager::onSettingInputClosed " << settingItemCode << value;
    DBRecord* r = settings.getItemByCode(settingItemCode);
    if (r != nullptr && value != (r->at(SettingDBTable::Value)).toString())
    {
        r->replace(SettingDBTable::Value, value);
        QString s = QString("Изменена настройка. Код: %1. Значение: %2").arg(QString::number(settingItemCode), value);
        emit log(LogType_Warning, LogSource_Admin, s);
        emit updateDBRecord(DataBase::ReplaceSettingsItem, *r);
    }
}

void AppManager::onAdminSettingsClicked()
{
    qDebug() << "@@@@@ AppManager::onAdminSettingsClicked";
    emit log(LogType_Info, LogSource_Admin, "Просмотр настроек");
    emit showSettingGroupsPanel();
}

void AppManager::beep()
{
    qDebug() << "@@@@@ AppManager::beep";
    QApplication::beep();
}

void AppManager::onLockClicked()
{
    qDebug() << "@@@@@ AppManager::onLockClicked";
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
    int v = Tools::stringToInt(value);
    if(v < 1)
    {
        v = 1;
        showMessage("ВНИМАНИЕ!", "Количество не должно быть меньше 1");
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
        authorizationOpened = false;
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
        showMessage("ВНИМАНИЕ!", "Ошибка базы данных!");
        return;
    }

    switch(selector)
    {
    case DataBase::GetSettings:
    // Обновление настроек:
        if(!records.isEmpty()) settings.updateAllItems(records);
        settingsPanelModel->update(settings);
        if(started) runEquipment(true);
        emit settingsChanged();
        break;

    case DataBase::GetAuthorizationUserByName:
    // Получен результат поиска пользователя по введеному имени при авторизации:
        checkAuthorization(records);
        break;

    case DataBase::GetShowcaseProducts:
    // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        emit selectFromDBByList(DataBase::GetShowcaseResources, records);
        break;

    case DataBase::GetMessageByResourceCode:
    // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty() && records[0].count() > ResourceDBTable::Value)
            showMessage("Описание товара", records[0][ResourceDBTable::Value].toString());
        break;

    case DataBase::GetUserNames:
    // Отображение имен пользователей при авторизации:
        showUsers(records);
        break;

    case DataBase::GetLog:
    // Отображение лога:
        if(!records.isEmpty()) viewLogPanelModel->update(records);
        break;

    case DataBase::GetProductsByGroupCodeIncludeGroups:
    // Отображение товаров выбранной группы:
        tablePanelModel->update(records);
        break;

    case DataBase::GetProductsByFilteredCode:
    // Отображение товаров с заданным фрагментом кода:
        searchPanelModel->update(records, SearchFilterModel::Code);
        break;

    case DataBase::GetProductsByFilteredBarcode:
    // Отображение товаров с заданным фрагментом штрих-кода:
        searchPanelModel->update(records, SearchFilterModel::Barcode);
        break;

    case DataBase::ReplaceSettingsItem:
        emit selectFromDB(DataBase::GetSettings, "");
        break;

    case DataBase::RefreshCurrentProduct:
        // Сброс выбранного товара после изменений в БД:
        resetProduct();
        if (!records.isEmpty() && !records.at(0).isEmpty())
        {
            //showToast("ВНИМАНИЕ!", "Выбранный товар изменен");
            setProduct(records.at(0));
        }
        break;

    case DataBase::GetShowcaseResources:
     // Отображение картинок товаров экрана Showcase:
    {
        QStringList fileNames;
        for (int i = 0; i < records.count(); i++) fileNames << getImageFileWithQmlPath(records[i]);
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DataBase::GetImageByResourceCode:
    // Отображение картинки выбранного товара:
    {
        QString imagePath = records.count() > 0 ? getImageFileWithQmlPath(records[0]) : DUMMY_IMAGE_FILE_WITH_QML_PATH;
        emit showProductImage(imagePath);
        //showMessage("Image file path", imagePath);
        break;
    }

    default:break;
    }
}

QString AppManager::getImageFileWithQmlPath(const DBRecord& r)
{
    QString path = DUMMY_IMAGE_FILE_WITH_QML_PATH;
    if (r.count() > ResourceDBTable::Value)
    {
        QString fileName = r[ResourceDBTable::Value].toString();
        if(Tools::fileExistsInAppPath(Tools::appFilePath(DATA_STORAGE_SUBDIR, fileName)))
            path = Tools::qmlFilePath(DATA_STORAGE_SUBDIR, fileName);
    }
    return path;
}

void AppManager::onViewLogClicked()
{
    emit log(LogType_Info, LogSource_Admin, "Просмотр лога");
    emit selectFromDB(DataBase::GetLog, "");
    emit showViewLogPanel();
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
    weightManager->setWeightParam(EquipmentParam_Tare);
    updateStatus();
}

void AppManager::onZeroClicked()
{
    qDebug() << "@@@@@ AppManager::onZeroClicked ";
    weightManager->setWeightParam(EquipmentParam_Zero);
    updateStatus();
}

void AppManager::onConfirmationClicked(const int selector)
{
    qDebug() << "@@@@@ AppManager::onConfirmationClicked " << selector;
    switch (selector)
    {
    case DialogSelector::Dialog_Authorization:
        startAuthorization();
        break;
    }
}

void AppManager::onTableResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onTableResultClicked " << index;
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

void AppManager::onTimer()
{
    quint64 dt = Tools::currentDateTimeToUInt() - userActionTime; // минуты
    quint64 bt = settings.getItemIntValue(SettingDBTable::SettingCode_Blocking); // минуты
    //qDebug() << "@@@@@ AppManager::onTimer " << dt << bt * 1000 * 60;
    if(!authorizationOpened && bt > 0 && bt * 1000 * 60 < dt)
        startAuthorization();
}

void AppManager::onSettingsItemClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSettingsItemClicked " << index;
    DBRecord* r =  settings.getItemByIndexInGroup(index);
    if (r != nullptr && !r->empty())
        emit showSettingInputBox((r->at(SettingDBTable::Code)).toInt(),
                                 (r->at(SettingDBTable::Name)).toString(),
                                 (r->at(SettingDBTable::Value)).toString());
}

void AppManager::onSettingGroupClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSettingGroupClicked " << index;
    settings.currentGroupIndex = index;
    settingsPanelModel->update(settings);
    emit showSettingsPanel(settings.getCurrentGroupName());
}

void AppManager::onSearchResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSearchResultClicked " << index;
    setProduct(searchPanelModel->productByIndex(index));
}

void AppManager::onShowcaseClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onShowcaseClicked " << index;
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
    emit selectFromDB(DataBase::GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::startAuthorization()
{
    qDebug() << "@@@@@ AppManager::startAuthorization";
    started = false;
    runEquipment(false);
    emit showAuthorizationPanel(appInfo.all());
    authorizationOpened = true;
    emit log(LogType_Warning, LogSource_User, "Авторизация");
    emit selectFromDB(DataBase::GetUserNames, "");
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    qDebug() << "@@@@@ AppManager::onCheckAuthorizationClick " << normalizedLogin << password;
    user[UserDBTable::Name] = normalizedLogin;
    user[UserDBTable::Password] = password;
    emit selectFromDB(DataBase::GetAuthorizationUserByName, normalizedLogin);
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
                showMessage(title, error);
                emit log(LogType_Warning, LogSource_User, QString("%1. %2").arg(title, error));
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
    emit log(LogType_Warning, LogSource_User, s);
    if(!started)
    {
        started = true;
        runEquipment(true);
        // showMessage(title, "Успешно!");
        emit authorizationSucceded();
        refreshAll();
        resetProduct();
        mainPageIndex = 0;
        emit showMainPage(mainPageIndex);
        authorizationOpened = false;
    }
}

void AppManager::refreshAll()
{
    // Обновить всё на экране

    qDebug() << "@@@@@ AppManager::refreshAll";
    emit showAdminMenu(UserDBTable::isAdmin(user));
    emit selectFromDB(DataBase::GetShowcaseProducts, "");
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

void AppManager::showMessage(const QString &title, const QString &text)
{
    emit showMessageBox(title, text, true);
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

void AppManager::runEquipment(const bool start)
{
    // Запустить/остановить оборудование
    if(start)
    {
        QString demoMessage = "";
        netServer->start(settings.getItemIntValue(SettingDBTable::SettingCode_TCPPort));
        QString url1;
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
            QString address = settings.getItemStringValue(SettingDBTable::SettingCode_WMAddress);
            QString boudrate =  QString::number(SettingDBTable::getBoudrate(settings.getItemIntValue(SettingDBTable::SettingCode_WMBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingDBTable::SettingCode_WMTimeout));
            url1 = QString("serial://%1?baudrate=%2&timeout=%3").arg(address, boudrate, timeout);
        }
        int e1 = weightManager->start(url1);

        QString url2;
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
            QString address = settings.getItemStringValue(SettingDBTable::SettingCode_PrinterAddress);
            QString boudrate =  QString::number(SettingDBTable::getBoudrate(settings.getItemIntValue(SettingDBTable::SettingCode_PrinterBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingDBTable::SettingCode_PrinterTimeout));
            url2 = QString("serial://%1?baudrate=%2&timeout=%3").arg(address, boudrate, timeout);
        }
        int e2 = printManager->start(url2);

        if(e1 != 0) showMessage("ВНИМАНИЕ!", QString("Ошибка весового модуля %1!\n%2").arg(
                                    QString::number(e1), weightManager->getErrorDescription(e1)));
        if(e2 != 0) showMessage("ВНИМАНИЕ!", QString("Ошибка принтера %1!\n%2").arg(
                                    QString::number(e2), printManager->getErrorDescription(e2)));
        if(!demoMessage.isEmpty()) showMessage("ВНИМАНИЕ!", demoMessage);
    }
    else
    {
        netServer->stop();
        weightManager->stop();
        printManager->stop();
    }
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
    emit log(LogType_Error, LogSource_Print, QString("Печать. Код товара: %1. Вес/Количество: %2").arg(
                newTransaction[TransactionDBTable::ItemCode].toString(),
                newTransaction[TransactionDBTable::Weight].toString()));
    emit transaction(newTransaction);
    if (settings.getItemIntValue(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Print)
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
        emit log(LogType_Error, LogSource_Weight, QString("Ошибка весового модуля. Код: %1. Описание: %2").arg(
                    QString::number(errorCode),
                    weightManager->getErrorDescription(errorCode)));
        break;
    case EquipmentParam_PrintError:
        emit log(LogType_Error, LogSource_Print, QString("Ошибка принтера. Код: %1. Описание: %2").arg(
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

    const bool isAutoPrint = settings.getItemBoolValue(SettingDBTable::SettingCode_PrintAuto) &&
           (!isPieceProduct || settings.getItemBoolValue(SettingDBTable::SettingCode_PrintAutoPcs));
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
                    if(wg >= settings.getItemIntValue(SettingDBTable::SettingCode_PrintAutoWeight)) print();
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











