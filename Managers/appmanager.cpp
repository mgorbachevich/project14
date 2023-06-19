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
#include "viewlogpanelmodel.h"
#include "settinggroupspanelmodel.h"
#include "searchfiltermodel.h"
#include "netserver.h"
#include "dbthread.h"
#include "tools.h"

AppManager::AppManager(QObject *parent, QQmlContext* context): QObject(parent)
{
    qDebug() << "@@@@@ AppManager::AppManager";
    this->context = context;
    mode = Mode::Mode_Start;
    user = UserDBTable::defaultAdmin();
    db = new DataBase(DB_FILENAME, settings, this);
    dbThread = new DBThread(db, this);
    netServer = new NetServer(this);

    connect(this, &AppManager::start, db, &DataBase::onStart);
    connect(this, &AppManager::printed, db, &DataBase::onPrinted);
    connect(this, &AppManager::saveLogInDB, db, &DataBase::onSaveLog);
    connect(this, &AppManager::selectFromDB, db, &DataBase::onSelect);
    connect(this, &AppManager::selectFromDBByList, db, &DataBase::onSelectByList);
    connect(this, &AppManager::updateDBRecord, db, &DataBase::onUpdateRecord);
    connect(this, &AppManager::download, db, &DataBase::onDownload);
    connect(this, &AppManager::upload, db, &DataBase::onUpload);
    connect(db, &DataBase::started, this, &AppManager::onDBStarted);
    connect(db, &DataBase::loadResult, this, &AppManager::onLoadResult, Qt::DirectConnection);
    connect(db, &DataBase::requestResult, this, &AppManager::onDBRequestResult);
    connect(db, &DataBase::downloadFinished, this, &AppManager::onDownloadFinished);
    connect(db, &DataBase::showMessageBox, this, &AppManager::showMessageBox);
    connect(netServer, &NetServer::netRequest, this, &AppManager::onNetRequest);

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
    QTimer::singleShot(10, this, &AppManager::start);
}

AppManager::~AppManager()
{
    dbThread->stop();
}

QString AppManager::versionAsString()
{
    return QString("Версия приложения %1. Версия БД %2").arg(APP_VERSION, DB_VERSION);
}

void AppManager::onDBStarted()
{
    showToast("", "Инициализация");
    qDebug() << "@@@@@ AppManager::onDBStarted";
    settings.createGroups((SettingGroupDBTable*)db->getTableByName(DBTABLENAME_SETTINGGROUPS));
    settingGroupsPanelModel->update(settings);
    startAuthorization();
    emit selectFromDB(DataBase::GetSettings, "");
    //showMessage("NetParams", QString("IP = %1").arg(Tools::getNetParams().localHostIP));
}

void AppManager::onDownloadFinished(const int count)
{
    qDebug() << "@@@@@ AppManager::onDownloadFinished ";
    refreshAll();
    showToast("Загрузка данных завершена", QString("Загружено записей: %1").arg(count));
    if(!currentProduct.isEmpty())
        emit selectFromDB(DataBase::GetCurrentProduct, currentProduct.at(ProductDBTable::Code).toString());
}

QString AppManager::priceAsString(const DBRecord& product)
{
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::moneyToText(productPrice(product), pp);
}

QString AppManager::weightOrPiecesAsString(const DBRecord& product)
{
    return ProductDBTable::isPiece(product) ? QString("%1").arg(pieces) : QString("%1").arg(weight, 0, 'f', 3);
}

QString AppManager::amountAsString(const DBRecord& product)
{
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    double v = 0;
    if(ProductDBTable::isPiece(product))
        v = pieces;
    else
        v = weight * (ProductDBTable::is100gBase(product) ? 10 : 1);
    return Tools::moneyToText(v * productPrice(product), pp);
}

double AppManager::productPrice(const DBRecord& product)
{
    const int p = ProductDBTable::Price;
    if (product.count() <= p)
        return 0;
    int pp = settings.getItemIntValue(SettingDBTable::SettingCode_PointPosition);
    return Tools::priceToDouble(product[p].toString(), pp);
}

void AppManager::onWeightParamChanged(const int param, const QString& value)
{
    switch (param)
    {
    case AppManager::WeightParam_AutoFlag:
    case AppManager::WeightParam_ZeroFlag:
    case AppManager::WeightParam_TareFlag:
    case AppManager::WeightParam_ErrorFlag:
        emit showWeightParam(param, value);
        break;
    case AppManager::WeightParam_TareValue:
        tare = Tools::stringToDouble(value);
        updateWeightPanel();
        break;
    case AppManager::WeightParam_WeightValue:
        weight = Tools::stringToDouble(value);
        updateWeightPanel();
        break;
    }
}

void AppManager::showCurrentProduct()
{
    qDebug() << "@@@@@ AppManager::showCurrentProduct " << currentProduct[ProductDBTable::Code].toString();
    productPanelModel->update(currentProduct, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    emit showProductPanel(ProductDBTable::isPiece(currentProduct));
    emit selectFromDB(DataBase::GetImageByResourceCode, currentProduct[ProductDBTable::PictureCode].toString());
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

void AppManager::filteredSearch()
{
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
            emit saveLogInDB(LogDBTable::LogType_Warning, "Неизвестный фильтр поиска");
            break;
    }
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
    if (tablePanelModel->groupUp()) updateTablePanel(false);
}

void AppManager::onSettingInputClosed(const int settingItemCode, const QString &value)
{
    qDebug() << "@@@@@ AppManager::onSettingInputClosed " << settingItemCode << value;
    DBRecord* r = settings.getItemByCode(settingItemCode);
    if (r != nullptr && value != (r->at(SettingDBTable::Value)).toString())
    {
        r->replace(SettingDBTable::Value, value);
        emit updateDBRecord(DataBase::ReplaceSettingsItem, *r);
    }
}

void AppManager::onAdminSettingsClicked()
{
    qDebug() << "@@@@@ AppManager::onAdminSettingsClicked";
    // todo
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
    emit activateMainPage(mainPageIndex);
}

void AppManager::onLoadResult(const qint64 requestId, const QString &json)
{
    qDebug() << "@@@@@ AppManager::onLoadResult " << requestId << json;
    netServer->makeReply(NetReply(requestId, json));
}

void AppManager::onNetRequest(const int requestType, const NetReply& p)
{
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
    pieces = Tools::stringToInt(value);
    updateWeightPanel();
}

void AppManager::onPopupClosed()
{
    if (--openedPopupCount <= 0)
    {
        openedPopupCount = 0;
        emit activateMainPage(mainPageIndex);
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
        netServer->start(settings.getItemIntValue(SettingDBTable::SettingCode_TCPPort));
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
            emit resetProduct();
            showMessage("ВНИМАНИЕ!", "Выбранный товар изменен");
        }
        break;

    default:break;
    }
}

void AppManager::onViewLogClicked()
{
    emit selectFromDB(DataBase::GetLog, "");
    emit showViewLogPanel();
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

void AppManager::updateTablePanel(const bool root)
{
    qDebug() << "@@@@@ AppManager::updateTablePanel";
    if (root) tablePanelModel->root();
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    emit selectFromDB(DataBase::GetProductsByGroupCodeIncludeGroups, currentGroupCode);
}

void AppManager::updateWeightPanel()
{
    QString c0 = "#404040";
    QString c1 = "#FFFFFF";
    const bool isPiece = ProductDBTable::isPiece(currentProduct);
    const bool is100gBase = ProductDBTable::is100gBase(currentProduct);

    emit showWeightParam(WeightParam::WeightParam_WeightTitle, isPiece ? "КОЛИЧЕСТВО, шт" : "МАССА, кг");
    QString w = weightOrPiecesAsString(currentProduct);
    emit showWeightParam(WeightParam::WeightParam_WeightValue, w);
    emit showWeightParam(WeightParam::WeightParam_WeightColor, Tools::stringToDouble(w) != 0 ? c1 : c0);

    QString pt = "ЦЕНА, руб";
    if (isPiece)
        pt += "/шт";
    else if (is100gBase)
        pt += "/100г";
    else if (!currentProduct.isEmpty())
        pt += "/кг";
    emit showWeightParam(WeightParam::WeightParam_PriceTitle, pt);
    QString p = priceAsString(currentProduct);
    emit showWeightParam(WeightParam::WeightParam_PriceValue, p);
    emit showWeightParam(WeightParam::WeightParam_PriceColor, Tools::stringToDouble(p) != 0 ? c1 : c0);

    emit showWeightParam(WeightParam::WeightParam_AmountTitle, "СТОИМОСТЬ, руб");
    QString a = amountAsString(currentProduct);
    emit showWeightParam(WeightParam::WeightParam_AmountValue, a);
    emit showWeightParam(WeightParam::WeightParam_AmountColor, Tools::stringToDouble(a) != 0 ? c1 : c0);
}

void AppManager::startAuthorization()
{
    qDebug() << "@@@@@ AppManager::startAuthorization";
    mode = Mode::Mode_Start;
    QString title = QString("%1. IP %2").arg(versionAsString(), Tools::getNetParams().localHostIP);
    emit showAuthorizationPanel(title);
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

void AppManager::checkAuthorization(const DBRecordList& users)
{
    qDebug() << "@@@@@ AppManager::checkAuthorization";
#ifdef CHECK_AUTHORIZATION
    if (users.isEmpty())
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        showMessage("Авторизация", "Неизвестный пользователь!");
        emit saveLogInDB(LogDBTable::LogType_Info, "Авторизация. Неизвестный пользователь");
        return;
    }
    const DBRecord& newUser = users[0];
    if (user[UserDBTable::Name] != newUser[UserDBTable::Name] ||
        user[UserDBTable::Password] != newUser[UserDBTable::Password])
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        showMessage("Авторизация", "Неверные имя пользователя или пароль!");
        emit saveLogInDB(LogDBTable::LogType_Info, "Авторизация. Неверные имя пользователя или пароль");
        return;
    }
    user.clear();
    user.append(newUser);
#else
    user = UserDBTable::defaultAdmin();
#endif

    qDebug() << "@@@@@ AppManager::checkAuthorization OK";
    emit saveLogInDB(LogDBTable::LogType_Info, "Авторизация. Пользователь " + user[UserDBTable::Name].toString());
    if(mode == Mode::Mode_Start)
    {
        mode = Mode::Mode_Scale;
        // showMessage("Авторизация", "Успешно!");
        emit authorizationSucceded();
        refreshAll();
        emit resetProduct();
        mainPageIndex = 0;
        emit activateMainPage(mainPageIndex);
    }
}

void AppManager::refreshAll()
{
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

void AppManager::saveTransaction()
{
#ifdef SAVE_TRANSACTION_ON_PRINT
    DBRecord r = ((TransactionDBTable*)db->getTableByName(DBTABLENAME_TRANSACTIONS))->createRecord();
    r[TransactionDBTable::DateTime] = Tools::currentDateTimeToInt();
    r[TransactionDBTable::User] = user[UserDBTable::Code];
    r[TransactionDBTable::ItemCode] = Tools::stringToInt(currentProduct[ProductDBTable::Code]);
    r[TransactionDBTable::LabelNumber] = 0; // todo
    r[TransactionDBTable::Weight] = weightOrPiecesAsString(currentProduct); // todo
    r[TransactionDBTable::Price] = priceAsString(currentProduct); // todo
    r[TransactionDBTable::Cost] = amountAsString(currentProduct); // todo
    r[TransactionDBTable::Price2] = 0; // todo
    r[TransactionDBTable::Cost2] = 0; // todo
    emit printed(r);
#endif
}

void AppManager::showUsers(const DBRecordList& records)
{
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
    currentProduct.clear();
    pieces = 1;
    updateWeightPanel();
}











