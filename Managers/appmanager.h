#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QThread>
#include "constants.h"
#include "database.h"
#include "httpclient.h"

class ProductPanelModel;
class TablePanelModel;
class ShowcasePanelModel;
class SearchPanelModel;
class SearchFilterModel;
class UserNameModel;
class QQmlContext;

class AppManager : public QObject
{
    Q_OBJECT

public:
    enum Mode
    {
        Start = 0,
        Scale,
    };

    explicit AppManager(QObject*, QQmlContext*);
    ~AppManager();

private:
    double price();
    QString priceAsString();
    QString weightAsString();
    QString amountAsString();
    void showCurrentProduct();
    void filteredSearch();
    void updateSearchFilter();
    void updateShowcasePanel();
    void updateTablePanel();
    void updateWeightPanel();
    void updateAuthorizationPanel();
    bool checkAuthorization(const QString&, const QString&);

    Mode mode = Mode::Start;
    double weight = 0;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SearchFilterModel* searchFilterModel;
    UserNameModel* userNameModel;

#ifdef BACKGROUND_DATABASE
    QThread dbThread;
#else
    DataBase* db;
#endif
#ifdef BACKGROUND_HTTP
    QThread httpThread;
#else
    HTTPClient* httpClient;
#endif

signals:
    void print();
    void showMessageBox(const QString&, const QString&);
    void showPrice(const QString&);
    void showAmount(const QString&);
    void showWeight(const QString&);
    void showPriceColor(const QString&);
    void showAmountColor(const QString&);
    void showWeightColor(const QString&);
    void showProductImage(const QString&);
    void showTablePanelTitle(const QString&);
    void showProductPanel();
    void showGroupHierarchyRoot(const bool);
    void showTableOptions();
    void showSearchOptions();
    void showAuthorizationPanel();
    void startDB();
    void select(const DataBase::Selector, const QString&);
    void selectByList(const DataBase::Selector, const DBRecordList&);

public slots:
    void onStart();
    void onDBStarted();
    void onPrint();
    void onShowMessageBox(const QString&, const QString&);
    void onWeightChanged(double);
    void onSearchFilterEdited(const QString&);
    void onSearchFilterClicked(const int);
    void onProductDescriptionClicked();
    void onSearchOptionsClicked() { emit showSearchOptions(); }
    void onSearchResultClicked(const int);
    void onShowcaseClicked(const int);
    void onTableResultClicked(const int);
    void onTableOptionsClicked() { emit showTableOptions(); }
    void onTableBackClicked();
    void onProductPanelClosed();
    void onAuthorizationPanelClosed(const QString&, const QString&);
    void onSelectFromDBResult(const DataBase::Selector, const DBRecordList&);
};

#endif // APPMANAGER_H
