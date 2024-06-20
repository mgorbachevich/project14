#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include <QKeyEvent>
#include "constants.h"
#include "database.h"
#include "moneycalculator.h"
#include "users.h"
#include "settings.h"
#include "appinfo.h"
#include "printstatus.h"

class ProductPanelModel;
class ViewLogPanelModel;
class ShowcasePanelModel3;
class SearchFilterModel;
class SearchPanelModel3;
class SettingsPanelModel3;
class UserNameModel;
class BaseListModel;
class QQmlContext;
class NetServer;
class AppInfo;
class EquipmentManager;
class InputProductCodePanelModel3;
class EditUsersPanelModel3;
class MoneyCalculator;

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QQmlContext*, const QSize&, QApplication*);
    void showConfirmation(const ConfirmSelector, const QString&, const QString&);
    void showConfirmation(const ConfirmSelector, const QString&);
    QString priceAsString(const DBRecord& r) { return moneyCalculator->priceAsString(r); }
    void netDownload(QHash<DBTable*, DBRecordList>, int&, int&);
    QString netDelete(const QString&, const QString&);
    QString netUpload(const QString&, const QString&, const bool);
    void onParseSetRequest(const QString&, QHash<DBTable*, DBRecordList>&);

    Q_INVOKABLE void beepSound();
    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void clickSound();
    Q_INVOKABLE void debugLog(const QString&);
    Q_INVOKABLE bool isAdmin() { return users->isAdmin(users->getCurrentUser()); }
    Q_INVOKABLE bool isAuthorizationOpened() { return mainPageIndex == MainPageIndex_Authorization; }
    Q_INVOKABLE bool isSettingsOpened() { return isSettings; }
    Q_INVOKABLE void onAddUserClicked();
    Q_INVOKABLE void onAdminSettingsClicked();
    Q_INVOKABLE bool onBackgroundDownloadClicked();
    Q_INVOKABLE void onCalendarClosed(const int, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onConfirmationClicked(const int);
    Q_INVOKABLE void onDeleteUserClicked(const QString&);
    Q_INVOKABLE void onEditUsersPanelClicked(const int);
    Q_INVOKABLE void onEditUsersPanelClose();
    Q_INVOKABLE void onHierarchyUpClicked();
    Q_INVOKABLE void onInfoClicked();
    Q_INVOKABLE void onInputUserClosed(const QString&, const QString&, const QString&, const bool);
    Q_INVOKABLE void onLockClicked();
    Q_INVOKABLE void onMainPageSwiped(const int);
    Q_INVOKABLE void onNumberClicked(const QString&);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onSetProductByCodeClicked(const QString&);
    Q_INVOKABLE void onPasswordInputClosed(const int, const QString&);
    Q_INVOKABLE void onPopupOpened(const bool);
    Q_INVOKABLE void onProductCodeEdited(const QString&);
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
    EquipmentManager* equipmentManager = nullptr;
    PrintStatus printStatus;
    bool isRefreshNeeded = false;

private:
    void createDefaultData();
    void createDefaultImages();
    bool isProduct() { return !product.isEmpty(); }
    QString getImageFileWithQmlPath(const DBRecord&);
    void print();
    void refreshAll();
    void onCustomSettingsItemClicked(const DBRecord&);
    void onEditUsersClicked();
    void resetProduct();
    void setMainPage(const int);
    void setProduct(const DBRecord&);
    void showAuthorizationUsers();
    void showDateInputPanel(const int, const bool);
    void showToast(const QString&, const QString&, const int delaySec = 5);
    void startAuthorization();
    void startAll();
    void startSettings();
    void stopAuthorization(const QString&, const QString&);
    void stopAll();
    void stopSettings();
    void updateSearch(const QString&, const bool hierarchyRoot = false);
    void updateShowcase();
    void updateSystemStatus();
    void updateSettings(const int);
    void updateWeightStatus();

    DataBase* db = nullptr;
    AppInfo appInfo;
    DBRecord product;
    bool isResetProductNeeded = false;
    MoneyCalculator* moneyCalculator = nullptr;

    // Таймер
    QTimer *timer = nullptr;
    quint64 netActionTime = 0;
    quint64 userActionTime = 0;

    // Сеть:
    NetServer* netServer = nullptr;

    // UI:
    QQmlContext* context = nullptr;
    int mainPageIndex = MainPageIndex_Authorization; // Авторизация
    int secret = 0;
    bool isSettings = false;
    int popups = 0;

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
    void hideToast();
    void previousSettings();
    void resetCurrentProduct();
    void setCurrentProductFavorite(const bool);
    void showCalendarPanel(const int, const QString&, const int, const int, const int, const int, const int, const int);
    void showCurrentUser(const int, const QString&);
    void showConfirmationBox(const int, const QString&, const QString&);
    void showDownloadPanel();
    void showDownloadProgress(const int);
    void showEditUsersPanel();
    void showEnvironmentStatus(const bool, const bool, const bool, const bool);
    void showHierarchyRoot(const bool);
    void showInputUserPanel(const QString&, const QString&, const QString&, const bool);
    void showMainPage(const int);
    void showMessageBox(const QString&, const QString&, const bool);
    void showPasswordInputBox(const int);
    void showPiecesInputBox(const int, const int);
    void showProductCodeInputBox(const QString&);
    void showProductPanel(const QString&, const bool);
    void showSearchHierarchy(const bool);
    void showSettingInputBox(const int, const QString&, const QString&);
    void showSettingComboBox(const int, const QString&, const int, const QString&, const QString&);
    void showSettingSlider(const int, const QString&, const int, const int, const int, const int);
    void showSettingsPanel(const QString&);
    void showShowcaseSort(const int, const bool);
    void showViewLogPanel();
    void showVirtualKeyboard(const int);
    void showControlParam(const int, const QString&);
    void start();

public slots:
    void onSelectResult(const DBSelector, const DBRecordList&, const bool);
    void onDBStarted();
    void onEnterChar(const QChar) { clickSound(); onUserAction(); }
    void onEnterKey(const Qt::Key) { clickSound(); onUserAction(); }
    void onEquipmentParamChanged(const int, const int);
    void onPrinted(const DBRecord&);
    void onTimer();
    void onNetCommand(const int, const QString&);
};

#endif // APPMANAGER_H
