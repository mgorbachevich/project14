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

class ProductPanelModel;
class TablePanelModel;
class ViewLogPanelModel;
class ShowcasePanelModel2;
class SearchFilterModel;
class SearchPanelModel;
class SettingsPanelModel;
class SettingGroupsPanelModel;
class UserNameModel;
class SettingItemListModel;
class QQmlContext;
class NetServer;
class AppInfo;
class WeightManager;
class PrintManager;
class InputProductCodePanelModel;

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QQmlContext*, const QSize&, QApplication*);

    Q_INVOKABLE void beepSound();
    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void clickSound();
    Q_INVOKABLE void debugLog(const QString&);
    Q_INVOKABLE void onAdminSettingsClicked();
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onConfirmationClicked(const int);
    Q_INVOKABLE void onInfoClicked();
    Q_INVOKABLE void onLockClicked();
    Q_INVOKABLE void onMainPageSwiped(const int);
    Q_INVOKABLE void onNumberClicked(const QString&);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onSetProductByCodeClicked(const QString&);
    Q_INVOKABLE void onProductCodeEdited(const QString&);
    Q_INVOKABLE void onPrintClicked();
    Q_INVOKABLE void onProductDescriptionClicked();
    Q_INVOKABLE void onProductPanelCloseClicked();
    Q_INVOKABLE void onProductPanelPiecesClicked();
    Q_INVOKABLE void onRewind();
    Q_INVOKABLE void onSearchFilterClicked(const int);
    Q_INVOKABLE void onSearchFilterEdited(const QString&);
    Q_INVOKABLE void onSearchResultClicked(const int);
    Q_INVOKABLE void onSettingInputClosed(const int, const QString&);
    Q_INVOKABLE void onSettingsItemClicked(const int);
    Q_INVOKABLE void onSettingsPanelCloseClicked();
    Q_INVOKABLE void onShowcaseClicked(const int);
    Q_INVOKABLE void onShowcaseSortClicked(const int);
    Q_INVOKABLE void onTableBackClicked();
    Q_INVOKABLE void onTableResultClicked(const int);
    Q_INVOKABLE void onTareClicked();
    Q_INVOKABLE void onUserAction();
    Q_INVOKABLE void onViewLogClicked();
    Q_INVOKABLE void onVirtualKeyboardSet(const int);
    Q_INVOKABLE void onWeightPanelClicked(const int);
    Q_INVOKABLE void onZeroClicked();

private:
    QString amountAsString(const DBRecord&);
    void createDefaultData();
    void filteredSearch();
    bool isAuthorizationOpened() { return mainPageIndex < 0; }
    bool isSettingsOpened() { return isSettings; }
    QString getImageFileWithQmlPath(const DBRecord&);
    double price(const DBRecord&);
    QString priceAsString(const DBRecord&);
    void print();
    QString quantityAsString(const DBRecord&);
    void refreshAll();
    void resetProduct();
    void onCustomSettingsItemClicked(const DBRecord&);
    void setMainPage(const int);
    void setProduct(const DBRecord&);
    void setShowcaseSort(const int);
    void showConfirmation(const DialogSelector, const QString&, const QString&);
    void showMessage(const QString&, const QString&);
    void showToast(const QString&, const QString&, const int delaySec = 5);
    void showUsers(const DBRecordList&);
    void startAuthorization();
    void startEquipment(const bool server = true, const bool weight = true, const bool printer = true);
    void startSettings();
    void stopAuthorization(const DBRecordList&);
    void stopEquipment(const bool server = true, const bool weight = true, const bool printer = true);
    void stopSettings();
    void updateSystemStatus();
    void updateTablePanel(const bool);
    void updateWeightStatus();

    AppInfo appInfo;

    // БД:
    DataBase* db = nullptr;
    Settings settings;
    DBRecord user;
    DBRecord product;

    // Сеть:
    NetServer* netServer = nullptr;
    quint64 netActionTime = 0;
    bool isRefreshNeeded = false;
    int netRoutes = 0;

    // Оборудование:
    WeightManager* weightManager = nullptr;
    PrintStatus printStatus;
    PrintManager* printManager = nullptr;

    // UI:
    QQmlContext* context = nullptr;
    quint64 userActionTime = 0;
    int mainPageIndex = -1; // Авторизация
    int secret = 0;
    int showcaseSort = Sort_Name;
    bool isSettings = false;

    // UI Models:
    ProductPanelModel* productPanelModel = nullptr;
    ShowcasePanelModel2* showcasePanelModel = nullptr;
    TablePanelModel* tablePanelModel = nullptr;
    SearchPanelModel* searchPanelModel = nullptr;
    SettingsPanelModel* settingsPanelModel = nullptr;
    SettingGroupsPanelModel* settingGroupsPanelModel = nullptr;
    SearchFilterModel* searchFilterModel = nullptr;
    UserNameModel* userNameModel = nullptr;
    ViewLogPanelModel* viewLogPanelModel = nullptr;
    SettingItemListModel* settingItemListModel = nullptr;
    InputProductCodePanelModel* inputProductCodePanelModel = nullptr;

signals:
    void closeLogView();
    void closeInputProductPanel();
    void enableManualPrint(const bool);
    void enableSetProductByInputCode(const bool);
    void hideToast();
    void previousSettings();
    void resetCurrentProduct();
    void setCurrentUser(const int, const QString&);
    void showDateTime(const QString&);
    void showAdminMenu(bool);
    void showConfirmationBox(const int, const QString&, const QString&);
    void showEnvironmentStatus(const bool, const bool, const bool, const bool);
    void showGroupHierarchyRoot(const bool);
    void showMainPage(const int);
    void showMessageBox(const QString&, const QString&, const bool);
    void showPiecesInputBox(const int);
    void showProductCodeInputBox(const QString&);
    void showPrinterMessage(const QString&);
    void showProductImage(const QString&);
    void showProductPanel(const QString&, const bool);
    void showSettingInputBox(const int, const QString&, const QString&);
    void showSettingComboBox(const int, const QString&, const int, const QString&);
    void showSettingSlider(const int, const QString&, const int, const int, const int, const int);
    void showSettingsPanel(const QString&);
    void showShowcaseSort(const int);
    void showTablePanelTitle(const QString&);
    void showViewLogPanel();
    void showVirtualKeyboard(const int);
    void showWeightParam(const int, const QString&);
    void start();

public slots:
    void onDBRequestResult(const DBSelector, const DBRecordList&, const bool);
    void onDBStarted();
    void onEnterChar(const QChar) { clickSound(); onUserAction(); }
    void onEnterKey(const Qt::Key) { clickSound(); onUserAction(); }
    void onEquipmentParamChanged(const int, const int);
    void onNetAction(const int);
    void onPrinted(const DBRecord&);
    void onShowMessage(const QString &title, const QString &text) { showMessage(title, text); }
    void onTimer();
};

#endif // APPMANAGER_H
