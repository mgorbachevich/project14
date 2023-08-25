#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include "constants.h"
#include "database.h"
#include "settings.h"
#include "appinfo.h"

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
class NetServer;
class AppInfo;
class WeightManager;
class PrintManager;

class AppManager : public QObject
{
    Q_OBJECT

public:
    enum DialogSelector
    {
        Dialog_None = 0,
        Dialog_Authorization,
    };

    explicit AppManager(QQmlContext*, QObject*);
    ~AppManager();
    double price(const DBRecord&);

private:
    QString priceAsString(const DBRecord&);
    QString amountAsString(const DBRecord&);
    QString quantityAsString(const DBRecord&);
    void setProduct(const DBRecord&);
    void filteredSearch();
    void updateTablePanel(const bool);
    void updateStatus();
    void startAuthorization();
    void checkAuthorization(const DBRecordList&);
    void showUsers(const DBRecordList&);
    void refreshAll();
    void showToast(const QString&, const QString&, const int delaySec = 5);
    void showMessage(const QString&, const QString&);
    void resetProduct();
    void runEquipment(const bool);
    void print();

    bool started = false;
    DataBase* db = nullptr;
    DBThread* dbThread = nullptr;
    NetServer* netServer = nullptr;
    WeightManager* weightManager = nullptr;
    PrintManager* printManager = nullptr;
    Settings settings;
    DBRecord user;
    AppInfo appInfo;
    DBRecord product;

    // UI:
    QQmlContext* context = nullptr;
    quint64 userActionTime = 0;
    int openedPopupCount = 0;
    int mainPageIndex = 0;
    int secret = 0;
    bool authorizationOpened = false;

    // Models:
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
    void authorizationSucceded();
    void download(const qint64, const QString&);
    void enableManualPrint(const bool);
    void hideMessageBox();
    void transaction(const DBRecord&);
    void resetCurrentProduct();
    void log(const int, const int, const QString&);
    void selectFromDB(const DataBase::Selector, const QString&);
    void selectFromDBByList(const DataBase::Selector, const DBRecordList&);
    void setCurrentUser(const int userIndex, const QString& userName);
    void settingsChanged();
    void showAdminMenu(bool);
    void showAuthorizationPanel(const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
    void showGroupHierarchyRoot(const bool);
    void showMainPage(const int);
    void showMessageBox(const QString&, const QString&, const bool);
    void showPiecesInputBox(const int);
    void showPrinterMessage(const QString&);
    void showProductImage(const QString&);
    void showProductPanel(const QString&, const bool);
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
    void upload(const qint64, const QString&, const QString&);

public slots:
    void onAdminSettingsClicked();
    void onBeep() { QApplication::beep(); }
    void onCheckAuthorizationClicked(const QString&, const QString&);
    void onConfirmationClicked(const int);
    void onDBRequestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void onDBStarted();
    void onDownloadFinished(const int);
    void onEquipmentParamChanged(const int, const int);
    void onLoadResult(const qint64, const QString&);
    void onLockClicked();
    void onMainPageChanged(const int);
    void onNetRequest(const int, const NetReply&);
    void onPiecesInputClosed(const QString&);
    void onPopupClosed();
    void onPopupOpened();
    void onPrintClicked();
    void onPrinted(const DBRecord&);
    void onProductDescriptionClicked();
    void onProductPanelCloseClicked();
    void onProductPanelPiecesClicked();
    void onRewind();
    void onSearchFilterClicked(const int);
    void onSearchFilterEdited(const QString&);
    void onSearchOptionsClicked();
    void onSearchResultClicked(const int);
    void onSettingGroupClicked(const int);
    void onSettingInputClosed(const int, const QString&);
    void onSettingsItemClicked(const int);
    void onShowcaseClicked(const int);
    void onSwipeMainPage(const int);
    void onTableBackClicked();
    void onTableOptionsClicked();
    void onTableResultClicked(const int);
    void onTareClicked();
    void onTimer();
    void onUserAction();
    void onViewLogClicked();
    void onWeightPanelClicked(const int);
    void onZeroClicked();
};

#endif // APPMANAGER_H
