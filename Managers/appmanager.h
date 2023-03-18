#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
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
class Net;

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
        WeightParam_Zero = 0,
        WeightParam_Tare = 1,
        WeightParam_Auto = 2,
    };

    enum WeightValue
    {
        WeightValue_Weight = 0,
        WeightValue_Price = 1,
        WeightValue_Amount = 2,
        WeightValue_WeightColor = 3,
        WeightValue_PriceColor = 4,
        WeightValue_AmountColor = 5,
        WeightValue_WeightTitle = 6,
        WeightValue_PriceTitle = 7,
        WeightValue_AmountTitle = 8,
    };

    enum DialogSelector
    {
        Dialog_None = 0,
        Dialog_Authorization,
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
    void saveTransaction();
    void showUsers(const DBRecordList&);
    void log(const int, const QString&);

    QQmlContext* context = nullptr;
    Mode mode = Mode::Mode_Start;
    DataBase* db = nullptr;
    QThread* dbThread = nullptr;
     double weight = 0;
    DBRecord user;
    DBRecord product;
    Settings settings;
    Net* net;
    int openedPopupCount = 0;

    ProductPanelModel* productPanelModel;
    ShowcasePanelModel* showcasePanelModel;
    TablePanelModel* tablePanelModel;
    SearchPanelModel* searchPanelModel;
    SettingsPanelModel* settingsPanelModel;
    SettingGroupsPanelModel* settingGroupsPanelModel;
    SearchFilterModel* searchFilterModel;
    UserNameModel* userNameModel;

signals:
    void activateMainWindow();
    void authorizationSucceded();
    void newData(const QString&);
    void print();
    void printed(const DBRecord&);
    void resetProduct();
    void saveLog(const DBRecord&);
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void sendHTTPClientGet(const QString&);
    void setCurrentUser(const int userIndex, const QString& userName);
    void setWeightParam(const int, const bool);
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
    void showWeightParam(const int, const bool);
    void showWeightValue(const int, const QString&);
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
    void onPopupClosed();
    void onPopupOpened() { openedPopupCount++; }
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
    void onShowMessageBox(const QString&, const QString&);
    void onTableBackClicked();
    void onTableOptionsClicked() { emit showTableOptions(); }
    void onTableResultClicked(const int);
    void onWeightParam(const int param) { emit setWeightParam(param, true); }
    void onUpdateDBResult(const DataBase::Selector, const bool);
    void onWeightChanged(double);
    void onWeightParamChanged(const int param, const bool value) { emit showWeightParam(param, value); }
};

#endif // APPMANAGER_H
