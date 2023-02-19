#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include <QThread>
#include <QSslSocket>
#include "appmanager.h"
#include "resourcedbtable.h"
#include "productdbtable.h"
#include "userdbtable.h"
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
    mode = Mode::Start;
    user = UserDBTable::defaultAdmin();

    // БД:
    dbThread = new QThread(this);
    db = new DataBase();
    db->moveToThread(dbThread);
    connect(dbThread, &QThread::finished, db, &QObject::deleteLater);
    connect(this, &AppManager::startDB, db, &DataBase::onStart);
    connect(this, &AppManager::selectFromDB, db, &DataBase::onSelect);
    connect(this, &AppManager::selectFromDBByList, db, &DataBase::onSelectByList);
    connect(this, &AppManager::updateInDB, db, &DataBase::onUpdate);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::selectResult, this, &AppManager::onSelectFromDBResult);
    connect(db, &DataBase::updateResult, this, &AppManager::onUpdateInDBResult);
    connect(db, &DataBase::showMessageBox, this, &AppManager::showMessageBox);
    dbThread->start();
    emit startDB();

    // Поддержка SSL:
    // https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ AppManager::AppManager Device supports OpenSSL:" << QSslSocket::supportsSsl();

    startHTTPClient(db);

    // Модели:
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

void AppManager::startHTTPClient(DataBase* db)
{
#ifdef HTTP_CLIENT
    if (httpClientThread == nullptr)
    {
        httpClientThread = new QThread();
        HTTPClient* httpClient = new HTTPClient();
        httpClient->moveToThread(httpClientThread);
        connect(httpClientThread, &QThread::finished, httpClient, &QObject::deleteLater);
        connect(httpClient, &HTTPClient::newData, db, &DataBase::onNewData);
        connect(httpClient, &HTTPClient::showMessageBox, this, &AppManager::showMessageBox);
        connect(this, &AppManager::sendHTTPClientGet, httpClient, &HTTPClient::onSendGet);
        httpClientThread->start();
    }
#endif
}

void AppManager::stopHTTPClient()
{
    if (httpClientThread != nullptr)
    {
        emit showMessageBox("", "HTTP Client stoped");
        httpClientThread->quit();
        httpClientThread->wait();
        delete httpClientThread;
        httpClientThread = nullptr;
    }
}

void AppManager::stopHTTPServer()
{
    if (httpServer != nullptr)
    {
        //qDebug() << "@@@@@ AppManager::stopHTTPServer";
        disconnect(httpServer, &HTTPServer::showMessageBox, this, &AppManager::showMessageBox);
        delete httpServer;
        httpServer = nullptr;
    }
}

void AppManager::startHTTPServer()
{
#ifdef HTTP_SERVER
    if (httpServer == nullptr)
    {
        const int newPort = getIntSettingsValueByCode(SettingDBTable::SettingCode_TCPPort);
        //emit showMessageBox("", "HTTP Server started");
        qDebug() << "@@@@@ AppManager::AppManager TCP port:" << newPort;
        httpServer = new HTTPServer(nullptr, newPort);
        connect(httpServer, &HTTPServer::showMessageBox, this, &AppManager::showMessageBox);
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
    return (price() == 0) ? PRICE_0 : Tools::moneyToText(price(), getIntSettingsValueByCode(SettingDBTable::SettingCode_PointPosition));
#endif
}

QString AppManager::amountAsString()
{
#ifdef WEIGHT_0_ALLWAYS
    return AMOUNT_0;
#else
    return (price() == 0) ?  AMOUNT_0 : Tools::moneyToText(weight * price(), getIntSettingsValueByCode(SettingDBTable::SettingCode_PointPosition));
#endif
}

double AppManager::price()
{
    return product.count() <= ProductDBTable::Price? 0: product[ProductDBTable::Price].toDouble();
}

void AppManager::onWeightChanged(double value)
{
#ifdef LOG_BACKGROUND_THREADS
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
    qDebug() << "@@@@@ AppManager::showCurrentProduct " << product[ProductDBTable::Columns::Code].toString();
    productPanelModel->update(product, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    emit showProductPanel();
    emit selectFromDB(DataBase::Selector::GetImageByResourceCode,
                      product[ProductDBTable::Columns::PictureCode].toString());
    updateWeightPanel();

    if (getIntSettingsValueByCode(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Time)
    {
        int resetTime = getIntSettingsValueByCode(SettingDBTable::SettingCode_ProductResetTime);
        if (resetTime > 0) QTimer::singleShot(resetTime * 1000, this, &AppManager::resetProduct);
    }
}

void AppManager::onProductDescriptionClicked()
{
    qDebug() << "@@@@@ AppManager::onProductDescriptionClicked";
    emit selectFromDB(DataBase::Selector::GetMessageByResourceCode,
                      product[ProductDBTable::Columns::MessageCode].toString());
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
    DBRecord* r = getSettingsItemByIndex(index);
    if (r != nullptr && value != (r->at(SettingDBTable::Columns::Value)).toString())
    {
        r->replace(SettingDBTable::Columns::Value, value);
        emit updateInDB(DataBase::Selector::ReplaceSettingsItem, *r);
    }
}

void AppManager::onAdminSettingsClicked()
{
    qDebug() << "@@@@@ AppManager::onAdminSettingsClicked";
    // todo
    emit showSettingsPanel();
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
            const int column = ResourceDBTable::Columns::Value;
            for (int i = 0; i < records.count(); i++)
            {
                QString fileName = DUMMY_IMAGE;
                if (records[i].count() > column)
                {
                    fileName = records[i][column].toString();
                    // if (!Tools::fileExists(fileName)) fileName = DUMMY_IMAGE; // todo
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
            QString fileName = DUMMY_IMAGE;
            if (records.count() > 0)
            {
                const int column = ResourceDBTable::Columns::Value;
                if (records[0].count() > column)
                {
                    fileName = records[0][column].toString();
                    // if (!Tools::fileExists(fileName)) fileName = DUMMY_IMAGE; // todo
                }
            }
            emit showProductImage(fileName);
            break;
        }

        case DataBase::Selector::GetMessageByResourceCode:
        // Отображение сообщения (описания) выбранного товара:
            if (!records.isEmpty())
            {
                const int messageValueColumn = ResourceDBTable::Columns::Value;
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
            break;
    }
}

void AppManager::onUpdateInDBResult(const DataBase::Selector selector, const bool result)
{
    qDebug() << "@@@@@ AppManager::onUpdateInDBResult " << selector << result;
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

DBRecord* AppManager::getSettingsItemByIndex(const int index)
{
    return (index >= 0 && index < settings.count()) ? &settings[index] : nullptr;
}

DBRecord* AppManager::getSettingsItemByCode(const int code)
{
    bool ok = false;
    for (int i = 0; i < settings.count(); i++)
    {
        DBRecord& ri = settings[i];
        if (ri[SettingDBTable::Columns::Code].toInt(&ok) == code && ok)
            return &ri;
    }
    return nullptr;
}

void AppManager::onSettingsItemClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSettingsItemClicked " << index;
    DBRecord* r = getSettingsItemByIndex(index);
    if (r != nullptr && !r->empty())
        emit showSettingInputBox(index,
                                 (r->at(SettingDBTable::Columns::Name)).toString(),
                                 (r->at(SettingDBTable::Columns::Value)).toString());
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
    emit showAuthorizationPanel();
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
        return;
    }
    const DBRecord& newUser = users[0];
    if (user[UserDBTable::Columns::Name] != newUser[UserDBTable::Columns::Name] ||
        user[UserDBTable::Columns::Password] != newUser[UserDBTable::Columns::Password])
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        emit showMessageBox("Авторизация", "Неверные имя пользователя или пароль!");
        return;
    }
    user.clear();
    user.append(newUser);
#else
    user = UserDBTable::defaultAdmin();
#endif

    qDebug() << "@@@@@ AppManager::checkAuthorization OK";
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
    emit sendHTTPClientGet("127.0.0.1:" + getStringSettingsValueByCode(SettingDBTable::SettingCode_TCPPort));
#endif
}

QString AppManager::getStringSettingsValueByCode(const SettingDBTable::SettingCode code)
{
    DBRecord* r = getSettingsItemByCode(code);
    return r != nullptr ? (r->at(SettingDBTable::Columns::Value)).toString() : "";
}

int AppManager::getIntSettingsValueByCode(const SettingDBTable::SettingCode code)
{
    return Tools::stringToInt(getStringSettingsValueByCode(code));
}

void AppManager::onSettingsChanged(const DBRecordList& records)
{
    qDebug() << "@@@@@ AppManager::onSettingsChanged";
    settings.clear();
    settings.append(records);
    settingsPanelModel->update(&settings);

    const int newPort = getIntSettingsValueByCode(SettingDBTable::SettingCode_TCPPort);
    if (httpServer != nullptr && httpServer->port != newPort) // Restart server
    {
       stopHTTPServer();
       startHTTPServer();
    }
}

void AppManager::onDBStarted()
{
    startAuthorization();
    emit selectFromDB(DataBase::GetSettings, "");
}

void AppManager::onPrinted()
{
    if (getIntSettingsValueByCode(SettingDBTable::SettingCode_ProductReset) == SettingDBTable::ProductReset_Print)
        emit resetProduct();
}

void AppManager::onResetProduct()
{
    product.clear();
    updateWeightPanel();
}

void AppManager::onCheckAuthorizationClicked(const QString& login, const QString& password)
{
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    qDebug() << "@@@@@ AppManager::onCheckAuthorizationClick " << normalizedLogin << password;
    user[UserDBTable::Columns::Name] = normalizedLogin;
    user[UserDBTable::Columns::Password] = password;
    emit selectFromDB(DataBase::GetAuthorizationUserByName, normalizedLogin);
}








