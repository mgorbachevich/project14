#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include "constants.h"
#include "database.h"
#include "settings.h"

class QThread;
class HTTPServer;
class ProductPanelModel;
class TablePanelModel;
class ShowcasePanelModel;
class SearchFilterModel;
class SearchPanelModel;
class SettingsPanelModel;
class SettingGroupsPanelModel;
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

    enum DialogSelector
    {
        None = 0,
        Authorization,
    };

    explicit AppManager(QObject*, QQmlContext*);
    ~AppManager();

private:
    void createDB();
    void createModels();
    double price();
    QString priceAsString();
    QString weightAsString();
    QString amountAsString();
    QString versionAsString();
    void showCurrentProduct();
    void filteredSearch();
    void updateTablePanel();
    void updateWeightPanel();
    void startAuthorization();
    void checkAuthorization(const DBRecordList&);
    void onSettingsChanged(const DBRecordList&);
    void startHTTPServer();
    void stopHTTPServer();
    void startHTTPClient(DataBase*);
    void stopHTTPClient();
    void saveTransaction();
    void log(const int, const QString&);

    QQmlContext* context = nullptr;
    Mode mode = Mode::Start;
    DataBase* db = nullptr;
    QThread* dbThread = nullptr;
    QThread* httpClientThread = nullptr;
    HTTPServer* httpServer = nullptr;
    double weight = 0;
    DBRecord user;
    DBRecord product;
    Settings settings;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SettingsPanelModel* settingsPanelModel;
    SettingGroupsPanelModel* settingGroupsPanelModel;
    SearchFilterModel* searchFilterModel;
    UserNameModel* userNameModel;

signals:
    void authorizationSucceded();
    void newData(const QString&);
    void print();
    void printed(const DBRecord&);
    void resetProduct();
    void saveLog(const DBRecord&);
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void sendHTTPClientGet(const QString&);
    void showAdminMenu(bool);
    void showAmount(const QString&);
    void showAmountColor(const QString&);
    void showAuthorizationPanel(const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
    void showGroupHierarchyRoot(const bool);
    void showMessageBox(const QString&, const QString&);
    void showPrice(const QString&);
    void showPriceColor(const QString&);
    void showProductImage(const QString&);
    void showProductPanel();
    void showSearchOptions();
    void showSettingGroupsPanel();
    void showSettingInputBox(const int, const QString&, const QString&);
    void showSettingsPanel(const QString&);
    void showTableOptions();
    void showTablePanelTitle(const QString&);
    void showWeight(const QString&);
    void showWeightColor(const QString&);
    void start();
    void updateDBRecord(const DataBase::Selector, const DBRecord&);

public slots:
    void onAdminSettingsClicked();
    void onCheckAuthorizationClicked(const QString&, const QString&);
    void onConfirmationClicked(const int);
    void onDBStarted();
    void onLockClicked();
    void onLog(const int type, const QString &comment) { log(type, comment); }
    void onPrint() { emit print(); }
    void onPrinted();
    void onProductDescriptionClicked();
    void onProductPanelCloseClicked() { emit resetProduct(); }
    void onResetProduct();
    void onSearchFilterClicked(const int);
    void onSearchFilterEdited(const QString&);
    void onSearchOptionsClicked() { emit showSearchOptions(); }
    void onSearchResultClicked(const int);
    void onSelectFromDBResult(const DataBase::Selector, const DBRecordList&);
    void onSettingGroupClicked(const int);
    void onSettingInputClosed(const int, const QString&);
    void onSettingsItemClicked(const int);
    void onShowcaseClicked(const int);
    void onShowMessageBox(const QString&, const QString&);
    void onTableBackClicked();
    void onTableOptionsClicked() { emit showTableOptions(); }
    void onTableResultClicked(const int);
    void onTare();
    void onUpdateDBResult(const DataBase::Selector, const bool);
    void onWeightChanged(double);
    void onZero();
};

#endif // APPMANAGER_H
