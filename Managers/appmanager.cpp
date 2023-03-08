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
#include "logdbtable.h"
#include "productpanelmodel.h"
#include "tablepanelmodel.h"
#include "usernamemodel.h"
#include "showcasepanelmodel.h"
#include "searchpanelmodel.h"
#include "settingspanelmodel.h"
#include "searchfiltermodel.h"
#include "tools.h"
#ifdef HTTP_SERVER
#include "httpserver.h"
#endif
#ifdef HTTP_CLIENT
#include "httpclient.h"
#endif

AppManager::AppManager(QObject *parent, QQmlContext* context): QObject(parent)
{
    qDebug() << "@@@@@ AppManager::AppManager";
    this->context = context;
    mode = Mode::Start;
    user = UserDBTable::defaultAdmin();
    createDB();
    createModels();
    startHTTPClient(db);
    QTimer::singleShot(10, this, &AppManager::start); //emit start();
}

AppManager::~AppManager()
{
    if (dbThread != nullptr)
    {
        dbThread->quit();
        dbThread->wait();
    }
    stopHTTPClient();
    stopHTTPServer();
}

void AppManager::createDB()
{
    dbThread = new QThread(this);
    db = new DataBase();
    db->moveToThread(dbThread);
    connect(dbThread, &QThread::finished, db, &QObject::deleteLater);
    connect(this, &AppManager::start, db, &DataBase::onStart);
    connect(this, &AppManager::printed, db, &DataBase::onPrinted);
    connect(this, &AppManager::saveLog, db, &DataBase::onSaveLog);
    connect(this, &AppManager::selectFromDB, db, &DataBase::onSelect);
    connect(this, &AppManager::selectFromDBByList, db, &DataBase::onSelectByList);
    connect(this, &AppManager::updateDBRecord, db, &DataBase::onUpdateRecord);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::selectResult, this, &AppManager::onSelectFromDBResult);
    connect(db, &DataBase::updateResult, this, &AppManager::onUpdateDBResult);
    connect(db, &DataBase::showMessageBox, this, &AppManager::showMessageBox);
    connect(db, &DataBase::log, this, &AppManager::onLog);
    dbThread->start();
}

void AppManager::createModels()
{
    productPanelModel = new ProductPanelModel(this);
    showcasePanelModel = new ShowcasePanelModel(this);
    tablePanelModel = new TablePanelModel(this);
    settingsPanelModel = new SettingsPanelModel(this);
    searchPanelModel = new SearchPanelModel(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("tablePanelModel", tablePanelModel);
    context->setContextProperty("settingsPanelModel", settingsPanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);
}

void AppManager::startHTTPClient(DataBase* db)
{
#if defined(HTTP_CLIENT) || defined(HTTP_SERVER)    // Поддержка SSL:
    // https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ AppManager::AppManager Device supports OpenSSL:" << QSslSocket::supportsSsl();
#endif

#ifdef HTTP_CLIENT
    if (httpClientThread == nullptr)
    {
        qDebug() << "@@@@@ AppManager::startHTTPClient";
        httpClientThread = new QThread();
        HTTPClient* httpClient = new HTTPClient();
        httpClient->moveToThread(httpClientThread);
        connect(httpClientThread, &QThread::finished, httpClient, &QObject::deleteLater);
        connect(httpClient, &HTTPClient::newData, db, &DataBase::onNewData);
        connect(httpClient, &HTTPClient::showMessageBox, this, &AppManager::showMessageBox);
        connect(httpClient, &HTTPClient::log, this, &AppManager::onLog);
        connect(this, &AppManager::sendHTTPClientGet, httpClient, &HTTPClient::onSendGet);
        httpClientThread->start();
    }
#endif
}

void AppManager::stopHTTPClient()
{
#ifdef HTTP_CLIENT
    if (httpClientThread != nullptr)
    {
        qDebug() << "@@@@@ AppManager::stopHTTPClient";
        emit showMessageBox("", "HTTP Client stoped");
        httpClientThread->quit();
        httpClientThread->wait();
        delete httpClientThread;
        httpClientThread = nullptr;
    }
#endif
}

void AppManager::stopHTTPServer()
{
#ifdef HTTP_SERVER
    if (httpServer != nullptr)
    {
        qDebug() << "@@@@@ AppManager::stopHTTPServer";
        disconnect(httpServer, &HTTPServer::showMessageBox, this, &AppManager::showMessageBox);
        delete httpServer;
        httpServer = nullptr;
    }
#endif
}

void AppManager::startHTTPServer()
{
#ifdef HTTP_SERVER
    if (httpServer == nullptr)
    {
        qDebug() << "@@@@@ AppManager::startHTTPServer";
        const int newPort = settings.getItemIntValue(SettingDBTable::SettingCode_TCPPort);
        //emit showMessageBox("", "HTTP Server started");
        qDebug() << "@@@@@ AppManager::AppManager TCP port:" << newPort;
        httpServer = new HTTPServer(nullptr, newPort);
        connect(httpServer, &HTTPServer::showMessageBox, this, &AppManager::showMessageBox);
        connect(httpServer, &HTTPServer::log, this, &AppManager::onLog);
    }
#endif
}

QString AppManager::weightAsString()
{
#ifdef WEIGHT_0_ALLWAYS
    return WEIGHT_0;
#else
    return Tools::weightToText(weight);
#endif
}

QString AppManager::priceAsString()
{
#ifdef WEIGHT_0_ALLWAYS
    return PRICE_0;
#else
    return (price() == 0) ? PRICE_0 : Tools::moneyToText(price(), settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition));
#endif
}

QString AppManager::amountAsString()
{
#ifdef WEIGHT_0_ALLWAYS
    return AMOUNT_0;
#else
    return (price() == 0) ?  AMOUNT_0 : Tools::moneyToText(weight * price(), settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition));
#endif
}

QString AppManager::versionAsString()
{
    return QString("Версия приложения %1. Версия БД %2").arg(APP_VERSION, DB_VERSION);
}

double AppManager::price()
{
    return product.count() <= ProductDBTable::Price? 0: product[ProductDBTable::Price].toDouble();
}

void AppManager::onWeightChanged(double value)
{
#ifdef DEBUG_LOG_BACKGROUND_THREADS
    qDebug() << "@@@@@ AppManager::onWeightChanged " << value;
#endif

    if (mode == Mode::Scale)
    {
        weight = value;
        updateWeightPanel();
    }
}

void AppManager::showCurrentProduct()
{
    qDebug() << "@@@@@ AppManager::showCurrentProduct " << product[ProductDBTable::Code].toString();
    productPanelModel->update(product, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    emit showProductPanel();
    emit selectFromDB(DataBase::Selector::GetImageByResourceCode,
                      product[ProductDBTable::PictureCode].toString());
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
    emit selectFromDB(DataBase::Selector::GetMessageByResourceCode, product[ProductDBTable::MessageCode].toString());
}

void AppManager::filteredSearch()
{
    QString& v = searchFilterModel->filterValue;
    qDebug() << "@@@@@ AppManager::filteredSearch " << v;
    if (!v.isEmpty())
    {
        switch(searchFilterModel->index)
        {
            case SearchFilterModel::FilterIndex::Code:
                emit selectFromDB(DataBase::Selector::GetProductsByFilteredCode, v);
                break;
            case SearchFilterModel::FilterIndex::Barcode:
                emit selectFromDB(DataBase::Selector::GetProductsByFilteredBarcode, v);
                break;
             default:
                qDebug() << "@@@@@ AppManager::filteredSearch ERROR";
                log(LogDBTable::LogType_Warning, "Неизвестный фильтр поиска");
                break;
        }
    }
}

void AppManager::onShowMessageBox(const QString& titleText, const QString& messageText)
{
    qDebug() << "@@@@@ AppManager::onShowMessageBox " << titleText << messageText;
    emit showMessageBox(titleText, messageText);
}

void AppManager::onSearchFilterEdited(const QString& value)
{
    qDebug() << "@@@@@ AppManager::onSearchFilterEdited " << value;
    searchFilterModel->filterValue = value;
    filteredSearch();
}

void AppManager::onSearchFilterClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSearchFilterClicked " << index;
    searchFilterModel->index = (SearchFilterModel::FilterIndex)index;
    filteredSearch();
}

void AppManager::onTableBackClicked()
{
    qDebug() << "@@@@@ AppManager::onTableBackClicked";
    if (tablePanelModel->groupUp()) updateTablePanel();
}

void AppManager::onSettingInputClosed(const int index, const QString &value)
{
    qDebug() << "@@@@@ AppManager::onSettingInputClosed " << index << value;
    DBRecord* r = settings.getItemByIndex(index);
    if (r != nullptr && value != (r->at(SettingDBTable::Value)).toString())
    {
        r->replace(SettingDBTable::Value, value);
        emit updateDBRecord(DataBase::Selector::ReplaceSettingsItem, *r);
    }
}

void AppManager::onAdminSettingsClicked()
{
    qDebug() << "@@@@@ AppManager::onAdminSettingsClicked";
    // todo
    emit showSettingsPanel();
}

void AppManager::onLockClicked()
{
    qDebug() << "@@@@@ AppManager::onLockClicked";
    emit showConfirmationBox(DialogSelector::Authorization, "Подтверждение", "Вы хотите сменить пользователя?");
}

void AppManager::onSelectFromDBResult(const DataBase::Selector selector, const DBRecordList& records)
{
    qDebug() << "@@@@@ AppManager::onSelectFromDBResult " << selector;
    switch(selector)
    {
    case DataBase::Selector::GetSettings:
    // Обновление настроек:
        onSettingsChanged(records);
        break;

    case DataBase::Selector::GetShowcaseProducts:
    // Обновление списка товаров экрана Showcase:
        showcasePanelModel->updateProducts(records);
        emit selectFromDBByList(DataBase::Selector::GetShowcaseResources, records);
        break;

       case DataBase::Selector::GetShowcaseResources:
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

        case DataBase::Selector::GetAuthorizationUserByName:
        // Получен результат поиска пользователя по введеному имени при авторизации:
            checkAuthorization(records);
            break;

        case DataBase::Selector::GetImageByResourceCode:
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

        case DataBase::Selector::GetMessageByResourceCode:
        // Отображение сообщения (описания) выбранного товара:
            if (!records.isEmpty())
            {
                const int messageValueColumn = ResourceDBTable::Value;
                if (records[0].count() > messageValueColumn)
                     emit showMessageBox("Описание товара", records[0][messageValueColumn].toString());
            }
            break;

        case DataBase::Selector::GetUserNames:
        // Отображение имен пользователей при авторизации:
            if (records.isEmpty()) // В базе нет пользователей. Добавить администратора по умолчанию:
            {
                DBRecordList users;
                users << UserDBTable::defaultAdmin();
                userNameModel->update(users);
            }
            else
                userNameModel->update(records);
            break;

        case DataBase::Selector::GetProductsByGroupCodeIncludeGroups:
        // Отображение товаров выбранной группы:
            tablePanelModel->update(records);
            break;

        case DataBase::Selector::GetProductsByFilteredCode:
        // Отображение товаров с заданным фрагментом кода:
            searchPanelModel->update(records, SearchFilterModel::FilterIndex::Code);
            break;

        case DataBase::Selector::GetProductsByFilteredBarcode:
        // Отображение товаров с заданным фрагментом штрих-кода:
            searchPanelModel->update(records, SearchFilterModel::FilterIndex::Barcode);
            break;

        default:
            qDebug() << "@@@@@ AppManager::onSelectFromDBResult ERROR unknown selector";
            log(LogDBTable::LogType_Warning, "Неизвестный ответ БД");
            break;
    }
}

void AppManager::onUpdateDBResult(const DataBase::Selector selector, const bool result)
{
    qDebug() << "@@@@@ AppManager::onUpdateDBResult " << selector << result;
    if (!result)
    {
        emit showMessageBox("ВНИМАНИЕ!", "Ошибка при сохранении данных!");
        return;
    }
    switch(selector)
    {
    case DataBase::Selector::ReplaceSettingsItem:
        emit selectFromDB(DataBase::GetSettings, "");
        break;
    default: break;
    }
}

void AppManager::onConfirmationClicked(const int selector)
{
    qDebug() << "@@@@@ AppManager::onConfirmationClicked " << selector;
    switch (selector)
    {
    case DialogSelector::Authorization:
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
            if (tablePanelModel->groupDown(clickedProduct)) updateTablePanel();
        }
        else
        {
            product = clickedProduct;
            showCurrentProduct();
        }
    }
}

void AppManager::onSettingsItemClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSettingsItemClicked " << index;
    DBRecord* r =  settings.getItemByIndex(index);
    if (r != nullptr && !r->empty())
        emit showSettingInputBox(index,
                                 (r->at(SettingDBTable::Name)).toString(),
                                 (r->at(SettingDBTable::Value)).toString());
}

void AppManager::onSearchResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSearchResultClicked " << index;
    product = searchPanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::onShowcaseClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onShowcaseClicked " << index;
    product = showcasePanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::updateTablePanel()
{
    qDebug() << "@@@@@ AppManager::updateTablePanel";
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    emit selectFromDB(DataBase::GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::updateWeightPanel()
{
    QString c0 = "#404040";
    QString c1 = "#FFFFFF";

    QString w = weightAsString();
    emit showWeight(w);
    emit showWeightColor((weight != 0 && w != WEIGHT_0) ? c1 : c0);

    QString p = priceAsString();
    emit showPrice(p);
    emit showPriceColor((p != PRICE_0) ? c1 : c0);

    QString a = amountAsString();
    emit showAmount(a);
    emit showAmountColor((a != AMOUNT_0) ? c1 : c0);
}

void AppManager::startAuthorization()
{
    qDebug() << "@@@@@ AppManager::startAuthorization";
    mode = Mode::Start;
    emit showAuthorizationPanel(versionAsString());
    emit selectFromDB(DataBase::GetUserNames, "");
}

void AppManager::checkAuthorization(const DBRecordList& users)
{
    qDebug() << "@@@@@ AppManager::checkAuthorization";
#ifdef CHECK_AUTHORIZATION
    if (users.isEmpty())
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        emit showMessageBox("Авторизация", "Неизвестный пользователь!");
        log(LogDBTable::LogType_Info, "Авторизация. Неизвестный пользователь");
        return;
    }
    const DBRecord& newUser = users[0];
    if (user[UserDBTable::Name] != newUser[UserDBTable::Name] ||
        user[UserDBTable::Password] != newUser[UserDBTable::Password])
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        emit showMessageBox("Авторизация", "Неверные имя пользователя или пароль!");
        log(LogDBTable::LogType_Info, "Авторизация. Неверные имя пользователя или пароль");
        return;
    }
    user.clear();
    user.append(newUser);
#else
    user = UserDBTable::defaultAdmin();
#endif

    qDebug() << "@@@@@ AppManager::checkAuthorization OK";
    log(LogDBTable::LogType_Info, "Авторизация. Пользователь " + user[UserDBTable::Name].toString());
    if(mode == Mode::Start)
    {
        mode = Mode::Scale;
        // emit showMessageBox("Авторизация", "Успешно!");
        emit authorizationSucceded();
        emit showAdminMenu(UserDBTable::isAdmin(user));
        emit selectFromDB(DataBase::GetShowcaseProducts, "");
        searchFilterModel->update();
        updateTablePanel();
    }

#ifdef HTTP_CLIENT_TEST
    emit sendHTTPClientGet("127.0.0.1:" + settings.getItemStringValue(SettingDBTable::SettingCode_TCPPort));
#endif
}

void AppManager::saveTransaction()
{
#ifdef SAVE_TRANSACTION_ON_PRINT
    DBRecord r = ((TransactionDBTable*)db->getTableByName(DBTABLENAME_TRANSACTIONS))->createRecord();
    r[TransactionDBTable::DateTime] = QDateTime::currentMSecsSinceEpoch();
    r[TransactionDBTable::User] = user[UserDBTable::Code];
    r[TransactionDBTable::ItemCode] = Tools::stringToInt(product[ProductDBTable::Code]);
    r[TransactionDBTable::LabelNumber] = 0; // todo
    r[TransactionDBTable::Weight] = weightAsString(); // todo
    r[TransactionDBTable::Price] = priceAsString(); // todo
    r[TransactionDBTable::Cost] = amountAsString(); // todo
    r[TransactionDBTable::Price2] = 0; // todo
    r[TransactionDBTable::Cost2] = 0; // todo
    emit printed(r);
#endif
}

void AppManager::log(const int type, const QString &comment)
{
#ifdef SAVE_LOG_IN_DB
    DBRecord r = ((LogDBTable*)db->getTableByName(DBTABLENAME_LOG))->createRecord();
    r[LogDBTable::DateTime] = QDateTime::currentMSecsSinceEpoch();
    r[LogDBTable::Type] = type;
    r[LogDBTable::Comment] = comment;
    emit saveLog(r);
#endif
}

void AppManager::onSettingsChanged(const DBRecordList& records)
{
    qDebug() << "@@@@@ AppManager::onSettingsChanged";
    settingsPanelModel->update(settings.updateItems(records));

#ifdef HTTP_SERVER
    const int newPort = settings.getItemIntValue(SettingDBTable::SettingCode_TCPPort);
    if (httpServer == nullptr || httpServer->port != newPort) // Start or restart server
    {
       stopHTTPServer();
       startHTTPServer();
    }
#endif
}

void AppManager::onDBStarted()
{
    qDebug() << "@@@@@ AppManager::onDBStarted";
    settings.createGroups((SettingGroupDBTable*)db->getTableByName(DBTABLENAME_SETTINGGROUPS));
    startAuthorization();
    emit selectFromDB(DataBase::GetSettings, "");
}

void AppManager::onPrinted()
{
    qDebug() << "@@@@@ AppManager::onPrinted";
    saveTransaction();
    if (settings.getItemIntValue(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Print)
        emit resetProduct();
}

void AppManager::onResetProduct()
{
    qDebug() << "@@@@@ AppManager::onResetProduct";
    product.clear();
    updateWeightPanel();
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    qDebug() << "@@@@@ AppManager::onCheckAuthorizationClick " << normalizedLogin << password;
    user[UserDBTable::Name] = normalizedLogin;
    user[UserDBTable::Password] = password;
    emit selectFromDB(DataBase::GetAuthorizationUserByName, normalizedLogin);
}

void AppManager::onZero()
{
    emit showMessageBox("", ">0<");
}

void AppManager::onTare()
{
    emit showMessageBox("", ">T<");
}









