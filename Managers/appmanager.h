#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include <QKeyEvent>
#include "constants.h"
#include "database.h"
#include "settings.h"
#include "appinfo.h"
#include "printstatus.h"

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

    explicit AppManager(QQmlContext*, QObject*, const QSize&);
    ~AppManager();
    double price(const DBRecord&);

    // Экранные размеры:
    Q_INVOKABLE int screenWidth() const { return screenSize.width(); }
    Q_INVOKABLE int screenHeight() const { return screenSize.height(); }
    Q_INVOKABLE int weightTitleFontSize() const { return (int)(DEFAULT_WEIGHT_TITLE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int weightValueFontSize() const { return (int)(DEFAULT_WEIGHT_VALUE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int largeFontSize() const { return (int)(DEFAULT_LARGE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int normalFontSize() const { return (int)(DEFAULT_NORMAL_FONT_SIZE * screenScale); }
    Q_INVOKABLE int buttonSize() const { return (int)(DEFAULT_BUTTON_SIZE * screenScale); }
    Q_INVOKABLE int editWidth() const { return (int)(DEFAULT_EDIT_WIDTH * screenScale); }
    Q_INVOKABLE int spacer() const { return (int)(DEFAULT_SPACER * screenScale); }

    Q_INVOKABLE void beep();
    Q_INVOKABLE void onAdminSettingsClicked();
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onConfirmationClicked(const int);
    Q_INVOKABLE void onLockClicked();
    Q_INVOKABLE void onMainPageChanged(const int);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onPopupClosed();
    Q_INVOKABLE void onPopupOpened();
    Q_INVOKABLE void onPrintClicked();
    Q_INVOKABLE void onProductDescriptionClicked();
    Q_INVOKABLE void onProductPanelCloseClicked();
    Q_INVOKABLE void onProductPanelPiecesClicked();
    Q_INVOKABLE void onRewind();
    Q_INVOKABLE void onSearchFilterClicked(const int);
    Q_INVOKABLE void onSearchFilterEdited(const QString&);
    Q_INVOKABLE void onSearchOptionsClicked();
    Q_INVOKABLE void onSearchResultClicked(const int);
    Q_INVOKABLE void onSettingGroupClicked(const int);
    Q_INVOKABLE void onSettingInputClosed(const int, const QString&);
    Q_INVOKABLE void onSettingsItemClicked(const int);
    Q_INVOKABLE void onShowcaseClicked(const int);
    Q_INVOKABLE void onSwipeMainPage(const int);
    Q_INVOKABLE void onTableBackClicked();
    Q_INVOKABLE void onTableOptionsClicked();
    Q_INVOKABLE void onTableResultClicked(const int);
    Q_INVOKABLE void onTareClicked();
    Q_INVOKABLE void onUserAction();
    Q_INVOKABLE void onViewLogClicked();
    Q_INVOKABLE void onWeightPanelClicked(const int);
    Q_INVOKABLE void onZeroClicked();

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
    DBRecord product;
    AppInfo appInfo;
    PrintStatus printStatus;

    // UI:
    QQmlContext* context = nullptr;
    quint64 userActionTime = 0;
    int openedPopupCount = 0;
    int mainPageIndex = 0;
    int secret = 0;
    bool authorizationOpened = false;
    QSize screenSize;
    double screenScale = 0;

    // Models:
    ProductPanelModel* productPanelModel = nullptr;
    ShowcasePanelModel* showcasePanelModel = nullptr;
    TablePanelModel* tablePanelModel = nullptr;
    SearchPanelModel* searchPanelModel = nullptr;
    SettingsPanelModel* settingsPanelModel = nullptr;
    SettingGroupsPanelModel* settingGroupsPanelModel = nullptr;
    SearchFilterModel* searchFilterModel = nullptr;
    UserNameModel* userNameModel = nullptr;
    ViewLogPanelModel* viewLogPanelModel = nullptr;

signals:
    void authorizationSucceded();
    void download(const qint64, const QString&);
    void enableManualPrint(const bool);
    void hideMessageBox();
    void log(const int, const int, const QString&);
    void resetCurrentProduct();
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
    void transaction(const DBRecord&);
    void updateDBRecord(const DataBase::Selector, const DBRecord&);
    void upload(const qint64, const QString&, const QString&);

public slots:
    void onDBRequestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void onDBStarted();
    void onDownloadFinished(const int);
    void onEquipmentParamChanged(const int, const int);
    void onLoadResult(const qint64, const QString&);
    void onNetRequest(const int, const NetReply&);
    void onPrinted(const DBRecord&);
    void onTimer();
};

#endif // APPMANAGER_H
