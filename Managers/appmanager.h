#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include "database.h"

class QThread;
class HTTPServer;
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
    void updateShowcasePanel() { emit selectFromDB(DataBase::Selector::ShowcaseProducts, ""); }
    void updateTablePanel();
    void updateWeightPanel();
    void updateAuthorizationPanel() { emit selectFromDB(DataBase::Selector::UserNames, ""); }
    void startAuthorization();
    void checkAuthorization(const DBRecordList&);

    Mode mode = Mode::Start;
    QThread* dbThread = nullptr;
    QThread* httpClientThread = nullptr;
    HTTPServer* httpServer = nullptr;
    double weight = 0;
    DBRecord user;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SearchFilterModel* searchFilterModel;
    UserNameModel* userNameModel;

signals:
    void print();
    void newData(const QString&);
    void showMessageBox(const QString&, const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
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
    void showAdminMenu(bool);
    void startDB();
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void authorizationSucceded();

public slots:
    void onStart() { emit startDB(); }
    void onDBStarted() { startAuthorization(); }
    void onPrint() { emit print(); }
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
    void onAdminSettingsClicked();
    void onLockClicked() { emit showConfirmationBox(ConfirmationSelector::Authorization, "Подтверждение", "Вы хотите сменить пользователя?"); }
    void onCheckAuthorizationClicked(const QString&, const QString&);
    void onSelectFromDBResult(const DataBase::Selector, const DBRecordList&);
    void onConfirmationClicked(const int);
};

#endif // APPMANAGER_H
