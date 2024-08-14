#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include <QKeyEvent>
#include "constants.h"
#include "database.h"
#include "moneycalculator.h"
#include "users.h"
#include "showcase.h"
#include "settings.h"
#include "status.h"
#include "netactionresult.h"
#include "netserver.h"
#include "equipmentmanager.h"

class ProductPanelModel;
class ViewLogPanelModel;
class ShowcasePanelModel3;
class SearchFilterModel;
class SearchPanelModel3;
class SettingsPanelModel3;
class UserNameModel;
class BaseListModel;
class QQmlContext;
class AppInfo;
class EquipmentManager;
class InputProductCodePanelModel3;
class EditUsersPanelModel3;
class MoneyCalculator;
class ScreenManager;
class QNetworkSettingsManager;

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QQmlContext*, const QSize&, QApplication*);
    ~AppManager() { stopAll(); }

    void showConfirmation(const ConfirmSelector, const QString&, const QString&);
    void showToast(const QString&, const int delaySec = SHOW_SHORT_TOAST_SEC);
    void showWait(const bool);
    QString priceAsString(const DBRecord& r) { return moneyCalculator->priceAsString(r); }
    void netDownload(QHash<DBTable*, DBRecordList>, int&, int&);
    QString netDelete(const QString&, const QString&);
    QString netUpload(const QString&, const QString&, const bool);
    void onParseSetRequest(const QString&, QHash<DBTable*, DBRecordList>&);
    void onNetResult(NetActionResult&);
    void updateInputCodeList();
    void updateSearch();

    Q_INVOKABLE void beepSound();
    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void debugLog(const QString&);
    Q_INVOKABLE bool isAdmin() { return Users::isAdmin(users->getCurrentUser()); }
    Q_INVOKABLE bool isAuthorizationOpened();
    Q_INVOKABLE bool isSettingsOpened() { return status.isSettings; }
    Q_INVOKABLE void onAddUserClicked();
    Q_INVOKABLE void onAdminSettingsClicked();
    Q_INVOKABLE bool onBackgroundDownloadClicked();
    Q_INVOKABLE void onCalendarClosed(const int, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onClick() { clickSound(); onUserAction(); }
    Q_INVOKABLE void onConfirmationClicked(const int, const QString&);
    Q_INVOKABLE void onDeleteUserClicked(const QString&);
    Q_INVOKABLE void onEditUsersPanelClicked(const int);
    Q_INVOKABLE void onEditUsersPanelClose();
    Q_INVOKABLE void onHelpClicked();
    Q_INVOKABLE void onHierarchyUpClicked();
    Q_INVOKABLE void onInfoClicked();
    Q_INVOKABLE void onInputUserClosed(const QString&, const QString&, const QString&, const bool);
    Q_INVOKABLE void onLockClicked();
    Q_INVOKABLE void onMainPageSwiped(const int i) { setMainPage(i); }
    Q_INVOKABLE void onNumberToSearchClicked(const QString&);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onSetProductByCodeClicked(const QString&);
    Q_INVOKABLE void onPasswordInputClosed(const int, const QString&);
    Q_INVOKABLE void onPopupOpened(const bool);
    Q_INVOKABLE void onInputProductCodeEdited(const QString&);
    Q_INVOKABLE void onPrintClicked();
    Q_INVOKABLE void onProductDescriptionClicked();
    Q_INVOKABLE void onProductFavoriteClicked();
    Q_INVOKABLE void onProductPanelCloseClicked();
    Q_INVOKABLE void onProductPanelPiecesClicked();
    Q_INVOKABLE void onRewind();
    Q_INVOKABLE void onSearchClicked();
    Q_INVOKABLE void onSearchFilterClicked(const int, const QString&);
    Q_INVOKABLE void onSearchFilterEdited(const QString&);
    Q_INVOKABLE void onSearchResultClicked(const int);
    Q_INVOKABLE void onSettingInputClosed(const int, const QString&);
    Q_INVOKABLE void onSettingsItemClicked(const int);
    Q_INVOKABLE void onSettingsPanelCloseClicked();
    Q_INVOKABLE void onShowcaseAutoClicked();
    Q_INVOKABLE void onShowcaseClicked(const int);
    Q_INVOKABLE void onShowcaseDirectionClicked();
    Q_INVOKABLE void onShowcaseSortClicked(const int);
    Q_INVOKABLE void onTareClicked();
    Q_INVOKABLE void onUserAction();
    Q_INVOKABLE void onViewLogClicked();
    Q_INVOKABLE void onVirtualKeyboardSet(const int);
    Q_INVOKABLE void onWeightPanelClicked(const int);
    Q_INVOKABLE void onZeroClicked();
    Q_INVOKABLE void showAttention(const QString& s) { showMessage("ВНИМАНИЕ!", s); }
    Q_INVOKABLE void showMessage(const QString&, const QString&);

    Settings* settings = nullptr;
    Users* users = nullptr;
    Showcase* showcase = nullptr;
    EquipmentManager* equipmentManager = nullptr;
    Status status;

private:
    void alarm();
    void clickSound();
    void createDefaultData();
    void createDefaultImages();
    bool isProduct() { return !product.isEmpty(); }
    DBRecord& getCurrentUser() { return users->getCurrentUser(); }
    QString getImageFileWithQmlPath(const DBRecord&);
    void print();
    void refreshAll();
    void onEditUsersClicked();
    void resetProduct();
    void setMainPage(const int);
    void setProduct(const DBRecord&);
    void setSettingsInfo();
    void setSettingsIPInfo();
    void showFoundProductsToast(const int);
    void showAuthorizationUsers();
    void showDateInputPanel(const int);
    void startAuthorization();
    void startAll();
    void startSettings();
    void stopAuthorization(const QString&, const QString&);
    void stopAll();
    void stopSettings();
    void updateShowcase();
    void updateSystemStatus();
    void updateSettings(const int);
    void updateWeightStatus();

    DataBase* db = nullptr;
    DBRecord product;
    bool isResetProductNeeded = false;
    MoneyCalculator* moneyCalculator = nullptr;
    QTimer *timer = nullptr;
    NetServer* netServer = nullptr;
    QQmlContext* context = nullptr;
    ScreenManager* screenManager = nullptr;
    QNetworkSettingsManager* networkSettingsManager = nullptr;

    // UI Models:
    ProductPanelModel* productPanelModel = nullptr;
    ShowcasePanelModel3* showcasePanelModel = nullptr;
    SearchPanelModel3* searchPanelModel = nullptr;
    SettingsPanelModel3* settingsPanelModel = nullptr;
    SearchFilterModel* searchFilterModel = nullptr;
    UserNameModel* userNameModel = nullptr;
    ViewLogPanelModel* viewLogPanelModel = nullptr;
    InputProductCodePanelModel3* inputProductCodePanelModel = nullptr;
    EditUsersPanelModel3* editUsersPanelModel = nullptr;
    BaseListModel* settingItemModel = nullptr;
    BaseListModel* calendarDayModel = nullptr;
    BaseListModel* calendarMonthModel = nullptr;
    BaseListModel* calendarYearModel = nullptr;
    BaseListModel* calendarHourModel = nullptr;
    BaseListModel* calendarMinuteModel = nullptr;
    BaseListModel* calendarSecondModel = nullptr;

signals:
    void closeLogView();
    void closeInputProductPanel();
    void enableManualPrint(const bool);
    void enableSetProductByInputCode(const bool);
    void hideToastBox();
    void previousSettings();
    void resetCurrentProduct();
    void setCurrentProductFavorite(const bool);
    void showCalendarPanel(const int, const QString&, const int, const int, const int, const int, const int, const int);
    void showCurrentUser(const int, const QString&);
    void showConfirmationBox(const int, const QString&, const QString&, const QString&);
    void showControlParam(const int, const QString&);
    void showDownloadPanel();
    void showDownloadProgress(const int);
    void showEditUsersPanel();
    void showEnvironmentStatus(const bool, const bool, const bool, const bool);
    void showHierarchyRoot(const bool);
    void showInputUserPanel(const QString&, const QString&, const QString&, const bool);
    void showMainPage(const int);
    void showMessageBox(const QString&, const QString&);
    void showPasswordInputBox(const int);
    void showPiecesInputBox(const int, const int);
    void showProductCodeInputBox(const QString&, const QString&);
    void showProductPanel(const QString&, const bool);
    void showSearchHierarchy(const bool);
    void showSettingInputBox(const int, const QString&, const QString&);
    void showSettingComboBox(const int, const QString&, const int, const QString&, const QString&);
    void showSettingSlider(const int, const QString&, const int, const int, const int, const int);
    void showSettingsPanel(const QString&);
    void showShowcaseSort(const int, const bool);
    void showToastBox(const QString&);
    void showViewLogPanel();
    void showVirtualKeyboard(const int);
    void showWaitBox(const bool);
    void start();

public slots:
    void onSelectResult(const DBSelector, const DBRecordList&, const bool);
    void onDBStarted();
    void onEquipmentParamChanged(const int, const int);
    void onPrinted(const DBRecord&);
    void onTimer();
    void onNetCommand(const int, const QString&);
};

#endif // APPMANAGER_H
