#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include "constants.h"
#include "database.h"

class QThread;
class HTTPServer;
class ProductPanelModel;
class TablePanelModel;
class ShowcasePanelModel;
class SearchFilterModel;
class SearchPanelModel;
class SettingsPanelModel;
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
    void updateSettingsPanel();
    void updateTablePanel();
    void updateWeightPanel();
    void startAuthorization();
    void checkAuthorization(const DBRecordList&);

    Mode mode = Mode::Start;
    QThread* dbThread = nullptr;
    QThread* httpClientThread = nullptr;
    HTTPServer* httpServer = nullptr;
    double weight = 0;
    DBRecord user;
    DBRecordList settings;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SettingsPanelModel* settingsPanelModel;
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
    void showSettingsPanel();
    void showAdminMenu(bool);
    void startDB();
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void authorizationSucceded();
    void sendHTTPClientGet(const QString&);

public slots:
    void onStart() { emit startDB(); }
    void onDBStarted();
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
    void onSettingsClicked(const int);
};

#endif // APPMANAGER_H
