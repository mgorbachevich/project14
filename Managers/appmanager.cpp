#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QQmlContext>
#include "appmanager.h"
#include "productdbtable.h"
#include "productdbtable.h"
#include "userdbtable.h"
#include "productpanelmodel.h"
#include "tablepanelmodel.h"
#include "usernamemodel.h"
#include "showcasepanelmodel.h"
#include "searchpanelmodel.h"
#include "searchfiltermodel.h"
#include "tools.h"
#include "resourcedbtable.h"

AppManager::AppManager(QObject *parent, QQmlContext* context): QObject(parent)
{
    qDebug() << "@@@@@ AppManager::AppManager";
    mode = Mode::Start;
    user = UserDBTable::defaultAdmin();

    // Инициализация БД:
#ifdef BACKGROUND_DATABASE
    DataBase* db = new DataBase();
    db->moveToThread(&dbThread);
    connect(&dbThread, &QThread::finished, db, &QObject::deleteLater);
#else
    db = new DataBase(this);
#endif
    connect(this, &AppManager::startDB, db, &DataBase::onStart);
    connect(this, &AppManager::select, db, &DataBase::onSelect);
    connect(this, &AppManager::selectByList, db, &DataBase::onSelectByList);
    connect(db, &DataBase::dbStarted, this, &AppManager::onDBStarted);
    connect(db, &DataBase::selectResult, this, &AppManager::onSelectFromDBResult);
#ifdef BACKGROUND_DATABASE
    dbThread.start();
#endif

    // Инициализация HTTP:
#ifdef BACKGROUND_HTTP
    HTTPClient* httpClient = new HTTPClient();
    httpClient->moveToThread(&httpThread);
    connect(&httpThread, &QThread::finished, httpClient, &QObject::deleteLater);
#else
    httpClient = new HTTPClient(this);
#endif
    connect(httpClient, &HTTPClient::getHTTPAnswer, db, &DataBase::onGetHTTPAnswer);
#ifdef BACKGROUND_HTTP
    httpThread.start();
#endif

    // Модели:
    productPanelModel = new ProductPanelModel(this, (ProductDBTable*)db->getTableByName(DBTABLENAME_PRODUCTS));
    showcasePanelModel = new ShowcasePanelModel(this);
    tablePanelModel = new TablePanelModel(this);
    searchPanelModel = new SearchPanelModel(this);
    searchFilterModel = new SearchFilterModel(this);
    userNameModel = new UserNameModel(this);
    context->setContextProperty("productPanelModel", productPanelModel);
    context->setContextProperty("showcasePanelModel", showcasePanelModel);
    context->setContextProperty("tablePanelModel", tablePanelModel);
    context->setContextProperty("searchPanelModel", searchPanelModel);
    context->setContextProperty("searchFilterModel", searchFilterModel);
    context->setContextProperty("userNameModel", userNameModel);

    // Подождем немного и стартуем:
    QTimer::singleShot(200, this, &AppManager::onStart);
}

AppManager::~AppManager()
{
#ifdef BACKGROUND_DATABASE
    dbThread.quit();
    dbThread.wait();
#endif
#ifdef BACKGROUND_HTTP
    httpThread.quit();
    httpThread.wait();
#endif
}

void AppManager::onStart()
{
    qDebug() << "@@@@@ AppManager::onStart";
    emit startDB();
}

void AppManager::onDBStarted()
{
    qDebug() << "@@@@@ AppManager::onDBStarted";
    emit showAuthorizationPanel();
    updateAuthorizationPanel();
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
    return (price() == 0) ? PRICE_0 : Tools::moneyToText(price());
#endif
}

QString AppManager::amountAsString()
{
#ifdef WEIGHT_0_ALLWAYS
    return AMOUNT_0;
#else
    return (price() == 0) ?  AMOUNT_0 : Tools::moneyToText(weight * price());
#endif
}

double AppManager::price()
{
    DBRecord& p = productPanelModel->product;
    return p.count() <= ProductDBTable::Price? 0: p[ProductDBTable::Price].toDouble();
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
    qDebug() << "@@@@@ AppManager::showCurrentProduct " << productPanelModel->product[ProductDBTable::Columns::Code].toString();
    productPanelModel->update();
    emit showProductPanel();
    emit select(DataBase::Selector::ImageByResourceCode,
                productPanelModel->product[ProductDBTable::Columns::PictureCode].toString());
    updateWeightPanel();
}

void AppManager::onProductDescriptionClicked()
{
    qDebug() << "@@@@@ AppManager::onProductDescriptionClicked";
    emit select(DataBase::Selector::MessageByResourceCode,
                productPanelModel->product[ProductDBTable::Columns::MessageCode].toString());
}

void AppManager::onSearchOptionsClicked()
{
    emit showSearchOptions();
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
                emit select(DataBase::Selector::ProductsByFilteredCode, v);
                break;
            case SearchFilterModel::FilterIndex::Barcode:
                emit select(DataBase::Selector::ProductsByFilteredBarcode, v);
                break;
             default:
                qDebug() << "@@@@@ AppManager::filteredSearch ERROR";
                break;
        }
    }
}

void AppManager::onPrint()
{
    qDebug() << "@@@@@ AppManager::onPrint";
    emit print();
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

void AppManager::onProductPanelClosed()
{
    productPanelModel->product.clear();
    updateWeightPanel();
}

void AppManager::onSelectFromDBResult(const DataBase::Selector selector, const DBRecordList& records)
{
    qDebug() << "@@@@@ AppManager::onSelectFromDBResult " << selector;
    switch(selector)
    {
        case DataBase::Selector::ShowcaseProducts:
        // Обновление списка товаров экрана Showcase:
            showcasePanelModel->updateProducts(records);
            emit selectByList(DataBase::Selector::ShowcaseResources, records);
            break;

        case DataBase::Selector::ShowcaseResources:
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

        case DataBase::Selector::AuthorizationUserByName:
        // Получен результат поиска пользователя по введеному имени при авторизации:
            if(records.isEmpty())
                emit showMessageBox("Авторизация", "Пользователь не найден!");
            else
                checkAuthorization(records[0]);
            break;

        case DataBase::Selector::ImageByResourceCode:
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

        case DataBase::Selector::MessageByResourceCode:
        // Отображение сообщения (описания) выбранного товара:
            if (!records.isEmpty())
            {
                const int messageValueColumn = ResourceDBTable::Columns::Value;
                if (records[0].count() > messageValueColumn)
                {
                    emit showMessageBox("Описание товара", records[0][messageValueColumn].toString());
                }
            }
            break;

        case DataBase::Selector::UserNames:
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

        case DataBase::Selector::ProductsByGroupCodeIncludeGroups:
        // Отображение товаров выбранной группы:
            tablePanelModel->update(records);
            break;

        case DataBase::Selector::ProductsByFilteredCode:
        // Отображение товаров с заданным фрагментом кода:
            searchPanelModel->update(records, SearchFilterModel::FilterIndex::Code);
            break;

        case DataBase::Selector::ProductsByFilteredBarcode:
        // Отображение товаров с заданным фрагментом штрих-кода:
            searchPanelModel->update(records, SearchFilterModel::FilterIndex::Barcode);
            break;

        default:
            qDebug() << "@@@@@ AppManager::onSelectFromDBResult ERROR unknown selector";
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
            productPanelModel->product = clickedProduct;
            showCurrentProduct();
        }
    }
}

void AppManager::onTableOptionsClicked()
{
    emit showTableOptions();
}

void AppManager::onSearchResultClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onSearchResultClicked " << index;
    productPanelModel->product = searchPanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::onShowcaseClicked(const int index)
{
    qDebug() << "@@@@@ AppManager::onShowcaseClicked " << index;
    productPanelModel->product = showcasePanelModel->productByIndex(index);
    showCurrentProduct();
}

void AppManager::updateSearchFilter()
{
    qDebug() << "@@@@@ AppManager::updateSearchFilter";
    searchFilterModel->update();
}

void AppManager::updateShowcasePanel()
{
    qDebug() << "@@@@@ AppManager::updateShowcasePanel";
    emit select(DataBase::Selector::ShowcaseProducts, "");
}

void AppManager::updateTablePanel()
{
    qDebug() << "@@@@@ AppManager::updateTablePanel";
    emit showTablePanelTitle(tablePanelModel->title());
    const QString currentGroupCode = tablePanelModel->lastGroupCode();
    emit showGroupHierarchyRoot(currentGroupCode.isEmpty() || currentGroupCode == "0");
    emit select(DataBase::Selector::ProductsByGroupCodeIncludeGroups, currentGroupCode);
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

void AppManager::updateAuthorizationPanel()
{
    qDebug() << "@@@@@ AppManager::updateAuthorizationPanel";
    emit select(DataBase::Selector::UserNames, "");
 }

void AppManager::checkAuthorization(const DBRecord& newUser)
{
    qDebug() << "@@@@@ AppManager::checkAuthorization";
    if (user[UserDBTable::Columns::Name] != newUser[UserDBTable::Columns::Name] ||
        user[UserDBTable::Columns::Password] != newUser[UserDBTable::Columns::Password])
    {
        qDebug() << "@@@@@ AppManager::checkAuthorization ERROR";
        emit showMessageBox("Авторизация", "Неверные имя пользователя или пароль!");
        return;
    }

    qDebug() << "@@@@@ AppManager::checkAuthorization OK";
    if(mode == Mode::Start)
    {
        mode = Mode::Scale;
        // emit showMessageBox("Авторизация", "Успешно!");
        emit authorizationSucceded();
        updateShowcasePanel();
        updateTablePanel();
        updateSearchFilter();
    }
}

void AppManager::onCheckAuthorizationClick(const QString& login, const QString& password)
{
    QString normalizedLogin = UserDBTable::fromAdminName(login);
    qDebug() << "@@@@@ AppManager::onCheckAuthorizationClick " << normalizedLogin << password;
    user[UserDBTable::Columns::Name] = normalizedLogin;
    user[UserDBTable::Columns::Password] = password;
    emit select(DataBase::Selector::AuthorizationUserByName, normalizedLogin);
}








