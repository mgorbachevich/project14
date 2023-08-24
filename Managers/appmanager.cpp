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
#include "netserver.h"
#include "dbthread.h"
#include "tools.h"
#include "appinfo.h"
#include "weightmanager.h"
#include "printmanager.h"

AppManager::AppManager(QQmlContext* context, QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ AppManager::AppManager";
    this->context = context;
    user = UserDBTable::defaultAdmin();
    db = new DataBase(DB_FILENAME, settings, this);
    dbThread = new DBThread(db, this);
    netServer = new NetServer(this);
    weightManager = new WeightManager(this);
    printManager = new PrintManager(this);
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &AppManager::onTimer);
    connect(this, &AppManager::start, db, &DataBase::onStart);
    connect(this, &AppManager::transaction, db, &DataBase::onTransaction);
    connect(this, &AppManager::log, db, &DataBase::onLog);
    connect(this, &AppManager::selectFromDB, db, &DataBase::onSelect);
    connect(this, &AppManager::selectFromDBByList, db, &DataBase::onSelectByList);
    connect(this, &AppManager::updateDBRecord, db, &DataBase::onUpdateRecord);
    connect(this, &AppManager::download, db, &DataBase::onDownload);
    connect(this, &AppManager::upload, db, &DataBase::onUpload);
    connect(db, &DataBase::started, this, &AppManager::onDBStarted);
    connect(db, &DataBase::loadResult, this, &AppManager::onLoadResult, Qt::DirectConnection);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(db, &DataBase::downloadFinished, this, &AppManager::onDownloadFinished);
    connect(netServer, &NetServer::netRequest, this, &AppManager::onNetRequest);
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

    appInfo.appVersion = APP_VERSION;
    appInfo.dbVersion = db->version();
    appInfo.weightManagerVersion = weightManager->version();
    appInfo.printManagerVersion = printManager->version();
    appInfo.netServerVersion = netServer->version();
    appInfo.ip = Tools::getNetParams().localHostIP;

    dbThread->start();
    QTimer::singleShot(10, this, &AppManager::start);
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

void AppManager::onDownloadFinished(const int count)
{
    qDebug() << "@@@@@ AppManager::onDownloadFinished" << count;
    refreshAll();
    //showToast("Загрузка данных завершена", QString("Загружено записей: %1").arg(count));
    if(!currentProduct.isEmpty())
        emit selectFromDB(DataBase::GetCurrentProduct, currentProduct.at(ProductDBTable::Code).toString());
}

QString AppManager::quantityAsString(const DBRecord& product)
{
    if(ProductDBTable::isPiece(product)) return QString("%1").arg(weightManager->getPieces());
    if(weightManager->isError()) return "";
    return QString("%1").arg(weightManager->getWeight(), 0, 'f', 3);
}

QString AppManager::priceAsString(const DBRecord& product)
{
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::moneyToText(price(product), pp);
}

QString AppManager::amountAsString(const DBRecord& product)
{
    double q = 0;
    if(ProductDBTable::isPiece(product))
        q = weightManager->getPieces();
    else if(!weightManager->isError())
        q = weightManager->getWeight() * (ProductDBTable::is100gBase(product) ? 10 : 1);
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::moneyToText(q * price(product), pp);
}

double AppManager::price(const DBRecord& product)
{
    const int p = ProductDBTable::Price;
    if (product.count() <= p)
        return 0;
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::priceToDouble(product[p].toString(), pp);
}

void AppManager::showCurrentProduct() // Вывод на экран панели выбранного продукта
{
    QString productCode = currentProduct[ProductDBTable::Code].toString();
    qDebug() << "@@@@@ AppManager::showCurrentProduct " << productCode;
    productPanelModel->update(currentProduct, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    emit showProductPanel(currentProduct[ProductDBTable::Name].toString(), ProductDBTable::isPiece(currentProduct));
    emit log(LogType_Info, LogSource_User, QString("Просмотр товара. Код: %1").arg(productCode));
    QString pictureCode = currentProduct[ProductDBTable::PictureCode].toString();
    emit selectFromDB(DataBase::GetImageByResourceCode, pictureCode);
    updateWeightPanel();

    if (settings.getItemIntValue(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Time)
    {
        int resetTime = settings.getItemIntValue(SettingDBTable::SettingCode_ProductResetTime);
        if (resetTime > 0) QTimer::singleShot(resetTime * 1000, this, &AppManager::resetProduct);
    }
}

void AppManager::onProductDescriptionClicked()
{
    qDebug() << "@@@@@ AppManager::onProductDescriptionClicked";
    emit selectFromDB(DataBase::GetMessageByResourceCode, currentProduct[ProductDBTable::MessageCode].toString());
}

void AppManager::onProductPanelCloseClicked()
{
    resetProduct();
}

void AppManager::onProductPanelPiecesClicked()
{
    emit showPiecesInputBox(weightManager->getPieces());
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

void AppManager::onLoadResult(const qint64 requestId, const QString &json)
{
    // БД завершила загрузку/выгрузку. Формируем ответ клиенту

    qDebug() << "@@@@@ AppManager::onLoadResult " << requestId << json;
    netServer->addReply(NetReply(requestId, json));
}

void AppManager::onNetRequest(const int requestType, const NetReply& p)
{
    // Получен запрос от клиета на загрузку/выгрузку. Формируем запрос в БД

    const qint64 requestId = p.first;
    const QString& query = p.second;
    qDebug() << (QString("@@@@@ AppManager::onNetRequest: request = \n%1, %2").arg(QString::number(requestId), query));
    if (netServer == nullptr)
    {
        qDebug() << "@@@@@ AppManager::onNetRequest: server == null";
        return;
    }
    switch(requestType)
    {
    case NetServer::GET: // Upload
    {
        // Parse list of codes to upload:
        qDebug() << "@@@@@ AppManager::onNetRequest: GET";
        QString codeList = "";
        QString source = "";
        DBTable* table = nullptr;
        const QString s1 = "source=";
        const QString s2 = "codes=";
        const QString s3 = "code=";
        int p1 = query.indexOf(s1);
        int p2 = query.indexOf("&");
        bool ok = p1 >= 0 && p2 > p1 && (query.contains(s2) || query.contains(s3));
        if(ok)
        {
            p1 += s1.length();
            source = query.mid(p1, p2 - p1); // table name
            qDebug() << "@@@@@ AppManager::onNetRequest: source =" << source;
            table = db->getTableByName(source);
            ok = table != nullptr;
        }
        if(ok)
        {
            if (query.contains(s2))
            {
                int p3 = query.indexOf("[") + 1;
                int p4 = query.indexOf("]");
                codeList = query.mid(p3, p4 - p3);
            }
            else if (query.contains(s3))
            {
                int p3 = query.indexOf(s3);
                int p4 = query.length();
                p3 += s3.length();
                codeList = query.mid(p3, p4 - p3);
            }
            else ok = false;
        }
        if(ok)
            emit upload(requestId, source, codeList);
        else
            qDebug() << "@@@@@ AppManager::onNetRequest: bad GET request";
        // reply = "{\"result\":\"0\",\"description\":\"ошибок нет\",\"data\":{\"products\":[]}}";
        break;
    }

    case NetServer::POST: // Download
    {
        qDebug() << "@@@@@ AppManager::onNetRequest: POST";
        int p1 = query.indexOf("{");
        int p2 = query.lastIndexOf("}") + 1;
        if (p1 >= 0 && p2 > p1)
        {
            QString json = query.mid(p1, p2 - p1);
            qDebug() << "@@@@@ AppManager::onNetRequest: json =" << json;
            emit download(requestId, json);
        }
        else
            qDebug() << "@@@@@ AppManager::onNetRequest: bad POST request";
        //reply = "{\"result\":\"0\",\"description\":\"ошибок нет\"}";
        break;
    }

    default:
        qDebug() << QString("@@@@@ AppManager::onNetRequest: ERROR unknown requst type");
        break;
    }
}

void AppManager::onPiecesInputClosed(const QString &value)
{
    qDebug() << "@@@@@ AppManager::onPiecesInputClosed " << value;
    weightManager->setPieces(Tools::stringToInt(value));
    updateWeightPanel();
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
        settings.updateAllItems(records);
        settingsPanelModel->update(settings);
        if(started) runEquipment(true);
        emit settingsChanged();
        break;

    case DataBase::GetShowcaseProducts:
    // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        emit selectFromDBByList(DataBase::GetShowcaseResources, records);
        break;

    case DataBase::GetShowcaseResources:
     // Отображение картинок товаров экрана Showcase:
    {
        QStringList fileNames;
        const int column = ResourceDBTable::Value;
        for (int i = 0; i < records.count(); i++)
        {
            QString fileName = DUMMY_IMAGE_FILE;
            if (records[i].count() > column)
            {
                fileName = records[i][column].toString();
                // if (!Tools::fileExists(fileName)) fileName = DUMMY_IMAGE_FILE; // todo
            }
            fileNames << fileName;
        }
        showcasePanelModel->updateImages(fileNames);
        break;
    }

    case DataBase::GetAuthorizationUserByName:
    // Получен результат поиска пользователя по введеному имени при авторизации:
        checkAuthorization(records);
        break;

    case DataBase::GetImageByResourceCode:
    // Отображение картинки выбранного товара:
    {
        QString fileName = DUMMY_IMAGE_FILE;
        if (records.count() > 0)
        {
            const int column = ResourceDBTable::Value;
            if (records[0].count() > column)
            {
                fileName = records[0][column].toString();
                // if (!Tools::fileExists(fileName)) fileName = DUMMY_IMAGE_FILE; // todo
            }
        }
        emit showProductImage(fileName);
        break;
    }

    case DataBase::GetMessageByResourceCode:
    // Отображение сообщения (описания) выбранного товара:
        if (!records.isEmpty())
        {
            const int messageValueColumn = ResourceDBTable::Value;
            if (records[0].count() > messageValueColumn)
                showMessage("Описание товара", records[0][messageValueColumn].toString());
        }
        break;

    case DataBase::GetUserNames:
    // Отображение имен пользователей при авторизации:
        showUsers(records);
        break;

    case DataBase::GetLog:
    // Отображение лога:
        viewLogPanelModel->update(records);
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

    case DataBase::GetCurrentProduct:
        if (!currentProduct.isEmpty() && !records.isEmpty() && !DBTable::isEqual(currentProduct, records.at(0)))
        {
            // Сброс выбранного товара после загрузки:
            resetProduct();
            showToast("ВНИМАНИЕ!", "Выбранный товар изменен");
        }
        break;

    default:break;
    }
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

void AppManager::onWeightParamClicked(const int param)
{
    qDebug() << "@@@@@ AppManager::onWeightParamClicked " << param;
    weightManager->setWeightParam(param);
    updateWeightPanel();
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
        DBRecord clickedProduct = tablePanelModel->productByIndex(index);
        if (ProductDBTable::isGroup(clickedProduct))
        {
            if (tablePanelModel->groupDown(clickedProduct)) updateTablePanel(false);
        }
        else
        {
            currentProduct = clickedProduct;
            showCurrentProduct();
        }
    }
}

void AppManager::onTimer()
{
    quint64 dt = Tools::currentDateTimeToInt() - userActionTime; // минуты
    quint64 bt = settings.getItemIntValue(SettingDBTable::SettingCode_Blocking); // минуты
    qDebug() << "@@@@@ AppManager::onTimer " << dt << bt * 1000 * 60;
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
    currentProduct = searchPanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::onShowcaseClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onShowcaseClicked " << index;
    currentProduct = showcasePanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::onSwipeMainPage(const int page)
{
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

#ifdef CHECK_AUTHORIZATION
    if (dbUsers.isEmpty())
        error = "Неизвестный пользователь";
    else
    {
        if (login != dbUsers[0][UserDBTable::Name].toString() || password != dbUsers[0][UserDBTable::Password])
            error = "Неверные имя пользователя или пароль";
    }
    if(!error.isEmpty())
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        showMessage(title, error);
        emit log(LogType_Warning, LogSource_User, QString("%1. %2").arg(title, error));
        return;
    }
    user.clear();
    user.append(dbUsers[0]);
#else
    user = UserDBTable::defaultAdmin();
    login = user[UserDBTable::Name].toString();
#endif

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
    QTimer::singleShot(delaySec * 1000, this, &AppManager::hideMessageBox);
}

void AppManager::showMessage(const QString &title, const QString &text)
{
    emit showMessageBox(title, text, true);
}

void AppManager::resetProduct()
{
    // Сбросить выбранный продукт

    qDebug() << "@@@@@ AppManager::resetProduct";
    currentProduct.clear();
    weightManager->setPieces(Tools::stringToInt(1));
    emit resetCurrentProduct();
    updateWeightPanel();
}

void AppManager::runEquipment(const bool start)
{
    // Запустить/остановить оборудование
    if(start)
    {
        netServer->start(settings.getItemIntValue(SettingDBTable::SettingCode_TCPPort));

        QString url1;
        bool demo1 = settings.getItemBoolValue(SettingDBTable::SettingCode_WMDemo);
        if(demo1)
            url1 = "demo://COM3?baudrate=115200&timeout=100"; // Demo
        else
        {
            QString address = settings.getItemStringValue(SettingDBTable::SettingCode_WMAddress);
            QString boudrate =  QString::number(SettingDBTable::getBoudrate(settings.getItemIntValue(SettingDBTable::SettingCode_WMBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingDBTable::SettingCode_WMTimeout));
            url1 = QString("serial://%1?baudrate=%2&timeout=%3").arg(address, boudrate, timeout);
        }
        int e1 = weightManager->start(url1, demo1);
        if(e1 != 0)
            showMessage("ВНИМАНИЕ!", QString("Ошибка весового модуля %1!\n%2").arg(
                            QString::number(e1),
                            weightManager->getErrorDescription(e1)));

        QString url2;
        bool demo2 = settings.getItemBoolValue(SettingDBTable::SettingCode_PrinterDemo);
        if(demo2)
            url2 = "demo://COM3?baudrate=115200&timeout=100";
        else
        {
            QString address = settings.getItemStringValue(SettingDBTable::SettingCode_PrinterAddress);
            QString boudrate =  QString::number(SettingDBTable::getBoudrate(settings.getItemIntValue(SettingDBTable::SettingCode_PrinterBaudrate)));
            QString timeout = QString::number(settings.getItemIntValue(SettingDBTable::SettingCode_PrinterTimeout));
            url2 = QString("serial://%1?baudrate=%20&timeout=%3").arg(address, boudrate, timeout);
        }
        int e2 = printManager->start(url2, demo2);
        if(e2 != 0)
            showMessage("ВНИМАНИЕ!", QString("Ошибка принтера %1!\n%2").arg(
                            QString::number(e2),
                            printManager->getErrorDescription(e2)));
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
    userActionTime = Tools::currentDateTimeToInt();
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

void AppManager::onPrint()
{
    // Печатаем этикетку

    qDebug() << "@@@@@ AppManager::onPrint ";
    printManager->print(db,
                        user,
                        currentProduct,
                        quantityAsString(currentProduct),
                        priceAsString(currentProduct),
                        amountAsString(currentProduct));
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
    updateWeightPanel();
}

void AppManager::updateWeightPanel()
{
    // Обновить весовую панель (вес, цена, флажки...)

    QString c0 = "#424242";
    QString c1 = "#fafafa";
    const bool isProduct = !currentProduct.empty();
    const bool isPiece = isProduct && ProductDBTable::isPiece(currentProduct);
    const bool is100g = isProduct && ProductDBTable::is100gBase(currentProduct);
    const bool isFixed = weightManager->isWeightFixed();
    const bool isError = weightManager->isError();
    const bool isZero = weightManager->isZeroFlag();
    const bool isTare = weightManager->isTareFlag() && (int)(weightManager->getTare() * 1000) != 0;
    const bool isAuto = isPiece ? settings.getItemBoolValue(SettingDBTable::SettingCode_PrintAutoPcs) :
                                  settings.getItemBoolValue(SettingDBTable::SettingCode_PrintAuto);
     // Флажки:
    emit showWeightParam(EquipmentParam_WeightError, Tools::boolToString(isError));
    emit showWeightParam(EquipmentParam_AutoPrint, Tools::boolToString(isAuto));
    emit showWeightParam(EquipmentParam_ZeroFlag, Tools::boolToString(isZero));
    emit showWeightParam(EquipmentParam_TareFlag, Tools::boolToString(isTare));

    // Загаловки:
    emit showWeightParam(EquipmentParam_WeightTitle, isPiece ? "КОЛИЧЕСТВО, шт" : "МАССА, кг");
    QString qt = "ЦЕНА, руб";
    if (isPiece) qt += "/шт";
    else if (is100g) qt += "/100г";
    else if (!currentProduct.isEmpty()) qt += "/кг";
    emit showWeightParam(EquipmentParam_PriceTitle, qt);
    emit showWeightParam(EquipmentParam_AmountTitle, "СТОИМОСТЬ, руб");

    // Вес/Штуки:
    QString q = (isPiece || !isError) ? quantityAsString(currentProduct) : "-----";
    emit showWeightParam(EquipmentParam_WeightValue, q);
    emit showWeightParam(EquipmentParam_WeightColor, isPiece || isFixed ? c1 : c0);

    // Цена:
    QString p = priceAsString(currentProduct);
    emit showWeightParam(EquipmentParam_PriceValue, p);
    emit showWeightParam(EquipmentParam_PriceColor, isProduct ? c1 : c0);

    // Стоимость:
    const bool isAmount = isProduct && (isPiece || (!isError && isFixed));
    QString a = amountAsString(currentProduct);
    emit showWeightParam(EquipmentParam_AmountValue, a);
    emit showWeightParam(EquipmentParam_AmountColor, isAmount ? c1 : c0);

    // Принтер
    if(printManager->isDemoMode())
        emit showPrinterMessage("Демо режим принтера");
    emit enablePrint(isAmount && !printManager->isError());

    qDebug() << QString("@@@@@ AppManager::updateWeightPanel %1 %2 %3 %4 %5 %6 %7 %8").arg(
                    Tools::boolToString(isError),
                    Tools::boolToString(isAuto),
                    Tools::boolToString(isTare),
                    Tools::boolToString(isZero),
                    Tools::boolToString(isFixed),
                    q, p, a);
}











