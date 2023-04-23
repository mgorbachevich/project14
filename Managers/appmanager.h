#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include "constants.h"
#include "database.h"
#include "settings.h"

class DBThread;
class ProductPanelModel;
class TablePanelModel;
class ViewLogPanelModel;
class ShowcasePanelModel;
class SearchFilterModel;
class SearchPanelModel;
class SettingsPanelModel;
class SettingGroupsPanelModel;
class UserNameModel;
class QQmlContext;
class TCPServer;

class AppManager : public QObject
{
    Q_OBJECT

public:
    enum Mode
    {
        Mode_Start = 0,
        Mode_Scale,
    };

    enum WeightParam
    {
        WeightParam_ZeroFlag = 0,
        WeightParam_TareFlag = 1,
        WeightParam_AutoFlag = 2,
        WeightParam_TareValue = 3,
        WeightParam_WeightValue = 4,
        WeightParam_PriceValue = 5,
        WeightParam_AmountValue = 6,
        WeightParam_WeightColor = 7,
        WeightParam_PriceColor = 8,
        WeightParam_AmountColor = 9,
        WeightParam_WeightTitle = 10,
        WeightParam_PriceTitle = 11,
        WeightParam_AmountTitle = 12,
    };

    enum DialogSelector
    {
        Dialog_None = 0,
        Dialog_Authorization,
    };

    explicit AppManager(QObject*, QQmlContext*);
    ~AppManager();

private:
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
    void saveTransaction();
    void showUsers(const DBRecordList&);
    void log(const int, const QString&);

    QQmlContext* context = nullptr;
    Mode mode = Mode::Mode_Start;
    DataBase* db = nullptr;
    DBThread* dbThread = nullptr;
    TCPServer* tcpServer = nullptr;
    Settings settings;
    DBRecord user;
    DBRecord product;
    double weight = 0;
    double tare = 0;
    int openedPopupCount = 0;
    int mainPageIndex = 0;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SettingsPanelModel* settingsPanelModel;
    SettingGroupsPanelModel* settingGroupsPanelModel;
    SearchFilterModel* searchFilterModel;
    UserNameModel* userNameModel;
    ViewLogPanelModel* viewLogPanelModel;

signals:
    void activateMainPage(const int index);
    void authorizationSucceded();
    void newData(const QString& json);
    void print();
    void printed(const DBRecord&);
    void resetProduct();
    void saveLog(const DBRecord&);
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void sendHTTPClientGet(const QString&);
    void setCurrentUser(const int userIndex, const QString& userName);
    void settingsChanged();
    void setWeightParam(const int);
    void showAdminMenu(bool);
    void showAuthorizationPanel(const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
    void showGroupHierarchyRoot(const bool);
    void showMainPage(const int);
    void showMessageBox(const QString&, const QString&);
    void showProductImage(const QString&);
    void showProductPanel();
    void showSearchOptions();
    void showSettingGroupsPanel();
    void showSettingInputBox(const int, const QString&, const QString&);
    void showSettingsPanel(const QString&);
    void showTableOptions();
    void showTablePanelTitle(const QString&);
    void showViewLogPanel();
    void showWeightParam(const int, const QString&);
    void start();
    void updateDBRecord(const DataBase::Selector, const DBRecord&);

public slots:
    void onAdminSettingsClicked();
    void onBeep() { QApplication::beep(); }
    void onCheckAuthorizationClicked(const QString&, const QString&);
    void onConfirmationClicked(const int);
    void onDBStarted();
    void onLockClicked();
    void onLog(const int type, const QString &comment) { log(type, comment); }
    void onMainPageChanged(const int);
    void onNewData(const QString& json) { emit newData(json); }
    void onPopupClosed();
    void onPopupOpened();
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
    void onShowMainPage(const int page) { emit showMainPage(page); }
    void onShowMessageBox(const QString& title, const QString& text) { emit showMessageBox(title, text); }
    void onTableBackClicked();
    void onTableOptionsClicked() { emit showTableOptions(); }
    void onTableResultClicked(const int);
    void onUpdateDBResult(const DataBase::Selector, const bool);
    void onViewLogClicked();
    void onWeightParamClicked(const int param) { emit setWeightParam(param); }
    void onWeightParamChanged(const int, const QString&);
};

#endif // APPMANAGER_H
