import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "constants.js" as Constants
import RegisteredTypes

ApplicationWindow
{
    id: mainWindow
    title: qsTr("ShtrihPrint6")
    visible: true
    Material.theme: Material.Light
    Material.background: Material.color(Material.Grey, Material.Shade100)
    Material.accent: Material.Orange
    color: Material.background
    property int pageIndicatorHeight: 16
    property int adminMenuWidth: 0
    property int popupWidth: mainWindow.width * 3 / 4
    property int popupHeight: mainWindow.height * 3 / 4
    property int popupX: (mainWindow.width - popupWidth) / 2
    property int popupY: (mainWindow.height - popupHeight) / 2

    Connections // Slot for signal AppManager::start:
    {
        target: app
        function onStart()
        {
            app.debugLog("@@@@@ mainWindow.onStart %1 %2".arg(screenManager.screenWidth()).arg(screenManager.screenHeight()));
            if (Qt.platform.os === "android") mainWindow.visibility = Window.FullScreen
            mainWindow.width = screenManager.screenWidth()
            mainWindow.height = screenManager.screenHeight()
        }
    }

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(value)
        {
            app.debugLog("@@@@@ mainWindow.onShowMainPage %1".arg(value));
            mainWindowLayout.visible = (value >= 0)
            authorizationPanel.visible = (value < 0)
            if(value >= 0) mainSwipeView.setCurrentIndex(value)
        }
    }

    Connections // Slot for signal AppManager::showAdminMenu:
    {
        target: app
        function onShowAdminMenu(show)
        {
            app.debugLog("@@@@@ mainWindow.onShowAdminMenu");
            adminMenuWidth = show ? screenManager.buttonSize() + screenManager.spacer() : 0
            adminMenuPanel.visible = show
        }
    }

    Connections // Slot for signal AppManager::showSettingInputBox:
    {
        target: app
        function onShowSettingInputBox(code, name, value)
        {
            app.debugLog("@@@@@ mainWindow.onShowSettingInputBox %1 %2 %3".arg(code).arg(name).arg(value));
            Qt.createComponent("Panels/inputSettingPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height,
                titleText: name, inputText: value, settingItemCode: code
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingComboBox:
    {
        target: app
        function onShowSettingComboBox(code, name, index, value)
        {
            app.debugLog("@@@@@ mainWindow.onShowSettingComboBox %1 %2 %3 %4".arg(code).arg(name).arg(index).arg(value));
            Qt.createComponent("Panels/comboSettingPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight,
                titleText: name, settingItemCode: code, comboIndex: index, comboText: value
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingSlider:
    {
        target: app
        function onShowSettingSlider(code, name, from, to, step, value)
        {
            app.debugLog("@@@@@ mainWindow.onShowSettingSlider %1 %2".arg(code).arg(name));
            Qt.createComponent("Panels/sliderSettingPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight,
                titleText: name, settingItemCode: code,
                sliderFrom: from, sliderTo: to, sliderStep: step, sliderValue: value
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showProductCodeInputBox:
    {
        target: app
        function onShowProductCodeInputBox(value)
        {
            app.debugLog("@@@@@ mainWindow.onShowProductCodeInputBox %1".arg(value));
            Qt.createComponent("Panels/inputProductCodePanel.qml").createObject(mainWindow,
            {
                x: popupX, y: screenManager.spacer() * 2,
                width: popupWidth, height: mainWindow.height - screenManager.spacer() * 4,
                inputText: value
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showPiecesInputBox:
    {
        target: app
        function onShowPiecesInputBox(value)
        {
            app.debugLog("@@@@@ mainWindow.onShowPiecesInputBox %1".arg(value));
            Qt.createComponent("Panels/inputPiecesPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight, inputText: value
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showViewLogPanel:
    {
        target: app
        function onShowViewLogPanel()
        {
            app.debugLog("@@@@@ mainWindow.onShowViewLogPanel");
            Qt.createComponent("Panels/viewLogPanel.qml").createObject(mainWindow,
            {
                x: 0, y: mainWeightPanel.height, width: mainWindow.width,
                height: mainWindow.height - mainWeightPanel.height
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showMessageBox:
    {
        target: app
        function onShowMessageBox(titleText, messageText, showButton)
        {
            app.debugLog("@@@@@ mainWindow.onShowMessageBox %1 %2".arg(titleText).arg(messageText));
            Qt.createComponent("Panels/messagePanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight, titleText: titleText,
                messageText: messageText, buttonVisibility: showButton
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showConfirmationBox:
    {
        target: app
        function onShowConfirmationBox(selector, titleText, messageText)
        {
            app.debugLog("@@@@@ mainWindow.onShowConfirmationBox %1 %2 %3".arg(selector).arg(titleText).arg(messageText));
            Qt.createComponent("Panels/confirmationPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight, titleText: titleText,
                messageText: messageText, dialogSelector: selector
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingsPanel:
    {
        target: app
        function onShowSettingsPanel(title)
        {
            app.debugLog("@@@@@ mainWindow.onShowSettingsPanel %1".arg(title));
            Qt.createComponent("Panels/settingsPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height, panelTitle: title
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showProductPanel:
    {
        target: app
        function onShowProductPanel(name, isPieceProduct)
        {
            app.debugLog("@@@@@ mainWindow.onShowProductPanel");
            Qt.createComponent("Panels/productPanel.qml").createObject(mainWindow,
            {
                x: 0, y: mainWeightPanel.height,
                width: mainWindow.width, height: mainWindow.height - mainWeightPanel.height,
                isPiece: isPieceProduct, productName: name
            }).open()
        }
    }

    Loader
    {
        id: authorizationPanel
        width: parent.width
        height: parent.height
        source: "Panels/authorizationPanel.qml"
    }

    Column
    {
        id: mainWindowLayout
        anchors.fill: parent
        spacing: 0
        padding: 0
        visible: false

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
                color: "transparent"

                Loader
                {
                    width: adminMenuWidth
                    source: "Panels/adminMenuPanel.qml"
                }
            }

            Rectangle
            {
                width: parent.width - adminMenuWidth
                height: parent.height
                color: "transparent"

                Rectangle
                {
                    width: parent.width
                    height: parent.height - pageIndicatorHeight
                    color: "transparent"

                    SwipeView
                    {
                        id: mainSwipeView
                        anchors.fill: parent
                        clip: true
                        currentIndex: 0
                        onCurrentIndexChanged: app.onMainPageSwiped(currentIndex);
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
                    color: "transparent"

                    PageIndicator
                    {
                        anchors.horizontalCenter: parent.horizontalCenter
                        count: mainSwipeView.count
                        currentIndex: mainSwipeView.currentIndex
                        Material.foreground: Material.color(Material.Grey, Material.Shade600)
                    }
                }
            }
        }
    }
}
