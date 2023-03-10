import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "constants.js" as Constants
import RegisteredTypes 1.0

ApplicationWindow
{
    // https://doc.qt.io/qt-5/qtquickcontrols2-material.html
    Material.theme: Material.Light
    Material.background: Material.color(Material.Grey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    Material.accent: Material.Orange
    Material.primary: Material.color(Material.Indigo, Material.Shade100)

    id: mainWindow
    objectName: "mainWindow"
    title: qsTr("Project14")
    color: Material.background
    visible: true

    property int pageIndicatorHeight: 16
    property int adminMenuWidth: 0
    property int popupX: (mainWindow.width - mainWindow.width * 4 / 5) / 2
    property int popupY: mainWindow.height / 4 + Constants.margin
    property int popupWidth: mainWindow.width * 4 / 5
    property int popupHeight: mainWindow.height * 2 / 3

    Connections // Slot for signal AppManager::start:
    {
        target: app
        function onStart()
        {
            console.debug("@@@@@ mainWindow.onStart");
            if (Qt.platform.os === "android")
                mainWindow.visibility = Window.FullScreen
            else
            {
                mainWindow.width = Constants.mainWindowWidth
                mainWindow.height = Constants.mainWindowHeight
            }
        }
    }

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(value)
        {
            console.debug("@@@@@ mainWindow.onShowMainPage");
            mainSwipeView.setCurrentIndex(value)
        }
    }

    Connections // Slot for signal AppManager::showAuthorizationPanel:
    {
        target: app
        function onShowAuthorizationPanel(value)
        {
            console.debug("@@@@@ mainWindow.onShowAuthorizationPanel");
            Qt.createComponent("Panels/authorizationPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: 0,
                                                                           y: 0,
                                                                           width: mainWindow.width,
                                                                           height: mainWindow.height,
                                                                           versionValue: value
                                                                       }).open()
        }
    }

    Connections // Slot for signal AppManager::showAdminMenu:
    {
        target: app
        function onShowAdminMenu(show)
        {
            console.debug("@@@@@ mainWindow.onShowAdminMenu");
            adminMenuWidth = show? Constants.buttonSize + Constants.margin * 2 : 0
            adminMenuPanel.visible = show
        }
    }

    Connections // Slot for signal AppManager::showSettingInputBox:
    {
        target: app
        function onShowSettingInputBox(code, name, value)
        {
            console.debug("@@@@@ settingPanel.showSettingInputBox ", code, " ", name, " ", value);
            Qt.createComponent("Panels/inputSettingPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: popupX,
                                                                           y: popupY,
                                                                           width: popupWidth,
                                                                           height: popupHeight,
                                                                           titleText: name,
                                                                           inputText: value,
                                                                           settingItemCode: code
                                                                       }).open()
        }
    }
    Connections // Slot for signal AppManager::showTableOptions:
    {
        target: app
        function onShowTableOptions()
        {
            console.debug("@@@@@ mainWindow.onShowTableOptions");
            Qt.createComponent("Panels/tableOptionsPanel.qml").createObject(mainWindow,
                                                                            {
                                                                                x: popupX,
                                                                                y: popupY,
                                                                                width: popupWidth,
                                                                                height: popupHeight
                                                                            }).open()
         }
    }

    Connections // Slot for signal AppManager::showSearchOptions:
    {
        target: app
        function onShowSearchOptions()
        {
            console.debug("@@@@@ mainWindow.onShowSearchOptions");
            Qt.createComponent("Panels/searchOptionsPanel.qml").createObject(mainWindow,
                                                                            {
                                                                                 x: popupX,
                                                                                 y: popupY,
                                                                                 width: popupWidth,
                                                                                 height: popupHeight
                                                                            }).open()
         }
    }

    Connections // Slot for signal AppManager::showMessageBox:
    {
        target: app
        function onShowMessageBox(titleText, messageText)
        {
            console.debug("@@@@@ mainWindow.onShowMessageBox ", titleText, " ", messageText);
            Qt.createComponent("Panels/messagePanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: popupX,
                                                                           y: popupY,
                                                                           width: popupWidth,
                                                                           height: popupHeight,
                                                                           titleText: titleText,
                                                                           messageText: messageText
                                                                       }).open()
        }
    }

    Connections // Slot for signal AppManager::showConfirmationBox:
    {
        target: app
        function onShowConfirmationBox(selector, titleText, messageText)
        {
            console.debug("@@@@@ mainWindow.onShowConfirmationBox ", selector, " ", titleText, " ", messageText);
            Qt.createComponent("Panels/confirmationPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: popupX,
                                                                           y: popupY,
                                                                           width: popupWidth,
                                                                           height: popupHeight,
                                                                           titleText: titleText,
                                                                           messageText: messageText,
                                                                           dialogSelector: selector
                                                                       }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingGroupsPanel:
    {
        target: app
        function onShowSettingGroupsPanel()
        {
            console.debug("@@@@@ mainWindow.onShowSettingGroupsPanel");
            Qt.createComponent("Panels/settingGroupsPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: 0,
                                                                           y: 0,
                                                                           width: mainWindow.width,
                                                                           height: mainWindow.height
                                                                       }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingsPanel:
    {
        target: app
        function onShowSettingsPanel(title)
        {
            console.debug("@@@@@ mainWindow.onShowSettingsPanel ", title);
            Qt.createComponent("Panels/settingsPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: 0,
                                                                           y: 0,
                                                                           width: mainWindow.width,
                                                                           height: mainWindow.height,
                                                                           panelTitle: title
                                                                       }).open()
        }
    }

    Connections // Slot for signal AppManager::showProductPanel:
    {
        target: app
        function onShowProductPanel()
        {
            console.debug("@@@@@ mainWindow.onShowProductPanel");
            Qt.createComponent("Panels/productPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: 0,
                                                                           y: mainWeightPanel.height,
                                                                           width: mainWindow.width,
                                                                           height: mainWindow.height - mainWeightPanel.height
                                                                       }).open()
         }
    }

    Column
    {
        anchors.fill: parent
        spacing: 0
        padding: 0

        Loader
        {
            id: mainWeightPanel
            width: parent.width
            source: "Panels/weightPanel.qml"
        }

        Row
        {
            width: parent.width
            height: parent.height - mainWeightPanel.height
            spacing: 0
            padding: 0

            Rectangle
            {
                id: adminMenuPanel
                width: adminMenuWidth
                height: parent.height
                color: Material.accent

                Column
                {
                    anchors.fill: parent
                    topPadding: Constants.margin
                    spacing: Constants.margin

                    Button
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: Constants.buttonSize
                        height: Constants.buttonSize
                        icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/build_black_48" }
                        leftInset: 0
                        topInset: 0
                        rightInset: 0
                        bottomInset: 0
                        Material.background: Material.primary
                        onClicked: app.onAdminSettingsClicked() // AppManager's slot
                    }

                    Button
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: Constants.buttonSize
                        height: Constants.buttonSize
                        icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/lock_black_48" }
                        leftInset: 0
                        topInset: 0
                        rightInset: 0
                        bottomInset: 0
                        Material.background: Material.primary
                        onClicked: app.onLockClicked() // AppManager's slot
                    }
                }
            }

            Rectangle
            {
                width: parent.width - adminMenuWidth
                height: parent.height

                Rectangle
                {
                    width: parent.width
                    height: parent.height - pageIndicatorHeight

                    SwipeView
                    {
                        id: mainSwipeView
                        anchors.fill: parent
                        currentIndex: 0
                        clip: true

                        focus: true
                        Loader
                        {
                            focus: true
                            source: "Panels/showcasePanel.qml"
                        }
                        Loader
                        {
                            focus: true
                            source: "Panels/tablePanel.qml"
                        }
                        Loader
                        {
                            focus: true
                            source: "Panels/searchPanel.qml"
                        }
                    }
                }

                Rectangle
                {
                    width: parent.width
                    height: pageIndicatorHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    color: Material.background

                    PageIndicator
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        count: mainSwipeView.count
                        currentIndex: mainSwipeView.currentIndex
                    }
                }
            }
        }
    }
}
