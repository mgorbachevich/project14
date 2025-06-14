#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QApplication>
#include <QObject>
#include <QKeyEvent>
#include "database.h"
#include "status.h"

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
class ScreenManager;
class QNetworkSettingsManager;
class Users;
class Showcase;
class NetServer;
class NetActionResult;
class CalendarModel;

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QQmlContext*, const QSize&, QApplication*);
    ~AppManager();

    void showConfirmation(const ConfirmSelector, const QString&, const QString&);
    void showToast(const QString&, const int delaySec = SHOW_SHORT_TOAST_SEC);
    void showWait(const bool, const bool modal = false);
    void netDownload(QHash<DBTable*, DBRecordList>, int&, int&);
    QString netDelete(const QString&, const QString&);
    QString netUpload(const QString&, const QString&, const bool);
    void onParseSetRequest(const QString&, QHash<DBTable*, DBRecordList>&);
    void onNetResult(NetActionResult&);
    void updateInputCodeList();
    void updateSearch();
    bool isProduct();
    int showcaseCount();
    DBRecord getShowcaseProductByIndex(const int);
    bool isProductInShowcase(const DBRecord&);
    void setSettingsNetInfo(const NetEntry&);
    void setSettingsNetInfo() { setSettingsNetInfo(Tools::getNetEntry()); }

    Q_INVOKABLE void beepSound();
    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void debugLog(const QString&);
    Q_INVOKABLE bool isAdmin();
    Q_INVOKABLE bool isAuthorizationOpened();
    Q_INVOKABLE bool isSettingsOpened();
    Q_INVOKABLE void onPopupOpened(const bool);
    Q_INVOKABLE void onCalendarClosed(const int, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
    Q_INVOKABLE void onEditUsersPanelClose();
    Q_INVOKABLE void onInputUserClosed(const QString&, const QString&, const QString&, const bool);
    Q_INVOKABLE void onPiecesInputClosed(const QString&);
    Q_INVOKABLE void onPasswordInputClosed(const int, const QString&);
    Q_INVOKABLE void onSettingInputClosed(const int, const QString&);
    Q_INVOKABLE void onMainPageSwiped(const int);
    Q_INVOKABLE void onInputProductCodeEdited(const QString&);
    Q_INVOKABLE void onSearchFilterEdited(const QString&);
    Q_INVOKABLE void onUserAction();
    Q_INVOKABLE void onVirtualKeyboardSet(const int);
    Q_INVOKABLE void onClick() { clickSound(); onUserAction(); }
    Q_INVOKABLE bool onClicked(const int);
    Q_INVOKABLE bool onClicked2(const int, const int);
    Q_INVOKABLE bool onClicked3(const int, const QString&);
    Q_INVOKABLE void onCheckAuthorizationClicked(const QString&, const QString&);
    Q_INVOKABLE void onConfirmationClicked(const int, const QString&);
    Q_INVOKABLE void onSearchFilterClicked(const int, const QString&);
    Q_INVOKABLE void onSettingsItemClicked(const int);
    Q_INVOKABLE void showAttention(const QString& s) { showMessage("ВНИМАНИЕ!", s); }
    Q_INVOKABLE void showMessage(const QString&, const QString&);

    Settings* settings = nullptr;
    Status status;

private:
    void alarm();
    void clickSound();
    void print();
    void refreshPanels();
    void resetProduct();
    void setMainPage(const int);
    void setProduct(const DBRecord&);
    void setProductByNumber(const QString&);
    bool setProductTare();
    void setSettingsInfo();
    void onSettingsChanged();
    void showAuthorizationUsers();
    void showDateInputPanel(const int);
    void showSettingComboBox2(const DBRecord&);
    void showTareToast(const bool showZero = true);
    void showWeightFlags();
    void startAuthorization();
    void startEquipment();
    void startSettings();
    void stopAuthorization(const QString&, const QString&);
    void stopEquipment();
    void stopSettings();
    void updateShowcase();
    void updateSettingsModel(const int);
    void update(const bool printIsPossible = true);
    bool isSelfService();

    EquipmentManager* equipmentManager = nullptr;
    DataBase* db = nullptr;
    DBRecord product;
    bool isResetProductNeeded = false;
    QTimer *timer = nullptr;
    NetServer* netServer = nullptr;
    QQmlContext* context = nullptr;
    ScreenManager* screenManager = nullptr;
    QNetworkSettingsManager* networkSettingsManager = nullptr;
    Users* users = nullptr;
    Showcase* showcase = nullptr;

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
    CalendarModel* calendarModel = nullptr;

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
    void showWaitBox(const bool, const bool);
    void start();

public slots:
    void onDBStarted();
    void onEquipmentParamChanged(const EquipmentParam, const int, const QString&);
    void onPrinted(const DBRecord&);
    void onTimer();
    void onNetCommand(const int, const QString&);
    void onSelfKeyPressed(const int);
};

#endif // APPMANAGER_H
