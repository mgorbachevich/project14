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
    Q_INVOKABLE void onAdminSettingsClicked();
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onConfirmationClicked(const int);
    Q_INVOKABLE void onInfoClicked();
    Q_INVOKABLE void onLockClicked();
    Q_INVOKABLE void onMainPageChanged(const int);
    Q_INVOKABLE void onNumberClicked(const QString&);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onSetProductByCodeClicked(const QString&);
    Q_INVOKABLE void onProductCodeEdited(const QString&);
    Q_INVOKABLE void onPopupClosed();
    Q_INVOKABLE void onPopupOpened();
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
    Q_INVOKABLE void onSwipeMainPage(const int);
    Q_INVOKABLE void onTableBackClicked();
    Q_INVOKABLE void onTableResultClicked(const int);
    Q_INVOKABLE void onTareClicked();
    Q_INVOKABLE void onUserAction();
    Q_INVOKABLE void onViewLogClicked();
    Q_INVOKABLE void onVirtualKeyboardSet(const int);
    Q_INVOKABLE void onWeightPanelClicked(const int);
    Q_INVOKABLE void onZeroClicked();

private:
    double price(const DBRecord&);
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
    void resetProduct();
    void startEquipment(const bool server = true, const bool weight = true, const bool printer = true);
    void stopEquipment(const bool server = true, const bool weight = true, const bool printer = true);
    void print();
    QString getImageFileWithQmlPath(const DBRecord&);
    void onCustomSettingsItemClicked(const DBRecord&);
    void setShowcaseSort(const int);

    bool isStarted = false;
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
    int openedPopupCount = 0;
    int mainPageIndex = 0;
    int secret = 0;
    bool isAuthorizationOpened = false;
    int showcaseSort = Sort_Name;

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
    void authorizationSucceded();
    void closeLogView();
    void closeSettings();
    void closeInputProductPanel();
    void enableManualPrint(const bool);
    void enableSetProductByInputCode(const bool);
    void hideToast();
    void resetCurrentProduct();
    void setCurrentUser(const int, const QString&);
    void showAdminMenu(bool);
    void showAuthorizationPanel(const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
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
    void onNetAction(const int);
    void onDBRequestResult(const DBSelector, const DBRecordList&, const bool);
    void onDBStarted();
    void onEquipmentParamChanged(const int, const int);
    void onPrinted(const DBRecord&);
    void onTimer();
    void onShowMessage(const QString &title, const QString &text) { emit showMessageBox(title, text, true); }
    void onEnterChar(const QChar) { clickSound(); onUserAction(); }
    void onEnterKey(const Qt::Key) { clickSound(); onUserAction(); }
};

#endif // APPMANAGER_H
