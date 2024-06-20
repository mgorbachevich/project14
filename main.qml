import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
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
    width: screenManager.screenWidth()
    height: screenManager.screenHeight()
    //property int adminMenuWidth: 0
    property int popupWidth: screenManager.popupWidth()
    property int popupHeight: screenManager.popupHeight()
    property int popupX: (width - popupWidth) / 2
    property int popupY: (height - popupHeight) / 2

    Connections // Slot for signal AppManager::start:
    {
        target: app
        function onStart()
        {
            //app.debugLog("@@@@@ mainWindow.onStart %1 %2".arg(screenManager.screenWidth()).arg(screenManager.screenHeight()));
            if (Qt.platform.os === "android") mainWindow.visibility = Window.FullScreen
        }
    }

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(value)
        {
            //app.debugLog("@@@@@ mainWindow.onShowMainPage %1".arg(value));
            switch(value)
            {
            case -1: // Authorization
                authorizationPanel.visible = true
                mainWindowLayout.visible = false
                mainWeightPanel.visible = false
                break
            case 0: // Showcase
                authorizationPanel.visible = false
                mainWindowLayout.visible = true
                mainWeightPanel.visible = true
                mainSwipeView.setCurrentIndex(value)
                break
            case 1: // Search
                authorizationPanel.visible = false
                mainWindowLayout.visible = true
                mainWeightPanel.visible = false
                mainSwipeView.setCurrentIndex(value)
                break
            }
        }
    }

    Connections // Slot for signal AppManager::showDownloadPanel:
    {
        target: app
        function onShowDownloadPanel()
        {
            //app.debugLog("@@@@@ mainWindow.onShowDownloadPanel");
            Qt.createComponent("Panels/downloadPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showCalendarPanel:
    {
        target: app
        function onShowCalendarPanel(code, title, day, month, year, hour, minute, second)
        {
            //app.debugLog("@@@@@ mainWindow.onShowCalendarPanel");
            Qt.createComponent("Panels/calendarPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight,
                code: code, titleText: title,
                calendarDay: day, calendarMonth: month, calendarYear: year,
                calendarHour: hour, calendarMinute: minute, calendarSecond: second
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingInputBox:
    {
        target: app
        function onShowSettingInputBox(code, name, value)
        {
            //app.debugLog("@@@@@ mainWindow.onShowSettingInputBox %1 %2 %3".arg(code).arg(name).arg(value));
            Qt.createComponent("Panels/inputSettingPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height,
                titleText: name, inputText: value, settingItemCode: code
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showPasswordInputBox:
    {
        target: app
        function onShowPasswordInputBox(code)
        {
            //app.debugLog("@@@@@ mainWindow.onShowPasswordInputBox %1".arg(code));
            Qt.createComponent("Panels/inputPasswordPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height, code: code
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingComboBox:
    {
        target: app
        function onShowSettingComboBox(code, name, index, value, comment)
        {
            //app.debugLog("@@@@@ mainWindow.onShowSettingComboBox %1 %2 %3 %4 %5".arg(code).arg(name).arg(index).arg(value).arg(comment));
            Qt.createComponent("Panels/comboSettingPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight,
                titleText: name, settingItemCode: code,
                comboIndex: index, comboText: value, commentText: comment
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingSlider:
    {
        target: app
        function onShowSettingSlider(code, name, from, to, step, value)
        {
            //app.debugLog("@@@@@ mainWindow.onShowSettingSlider %1 %2".arg(code).arg(name));
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
            //app.debugLog("@@@@@ mainWindow.onShowProductCodeInputBox %1".arg(value));
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
        function onShowPiecesInputBox(number, maxDigits)
        {
            //app.debugLog("@@@@@ mainWindow.onShowPiecesInputBox");
            Qt.createComponent("Panels/inputPiecesPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight, inputText: number, maxChars: maxDigits
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showViewLogPanel:
    {
        target: app
        function onShowViewLogPanel()
        {
            //app.debugLog("@@@@@ mainWindow.onShowViewLogPanel");
            Qt.createComponent("Panels/viewLogPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showMessageBox:
    {
        target: app
        function onShowMessageBox(titleText, messageText, showButton)
        {
            //app.debugLog("@@@@@ mainWindow.onShowMessageBox %1 %2".arg(titleText).arg(messageText));
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
            //app.debugLog("@@@@@ mainWindow.onShowConfirmationBox %1 %2 %3".arg(selector).arg(titleText).arg(messageText));
            Qt.createComponent("Panels/confirmationPanel.qml").createObject(mainWindow,
            {
                x: popupX, y: popupY, width: popupWidth, height: popupHeight, titleText: titleText,
                messageText: messageText, confirmSelector: selector
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showSettingsPanel:
    {
        target: app
        function onShowSettingsPanel(title)
        {
            //app.debugLog("@@@@@ mainWindow.onShowSettingsPanel %1".arg(title));
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
            //app.debugLog("@@@@@ mainWindow.onShowProductPanel");
            mainWeightPanel.visible = true
            Qt.createComponent("Panels/productPanel.qml").createObject(mainWindow,
            {
                x: 0, y: mainWeightPanel.height,
                width: mainWindow.width, height: mainWindow.height - mainWeightPanel.height,
                isPiece: isPieceProduct, productName: name
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showEditUsersPanel:
    {
        target: app
        function onShowEditUsersPanel()
        {
            //app.debugLog("@@@@@ mainWindow.onShowEditUsersPanel");
            Qt.createComponent("Panels/editUsersPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height
            }).open()
        }
    }

    Connections // Slot for signal AppManager::showInputUserPanel:
    {
        target: app
        function onShowInputUserPanel(code, name, password, admin)
        {
            //app.debugLog("@@@@@ mainWindow.onShowInputUserPanel");
            Qt.createComponent("Panels/inputUserPanel.qml").createObject(mainWindow,
            {
                x: 0, y: 0, width: mainWindow.width, height: mainWindow.height,
                userCode: code, userName: name, userPassword: password, isUserAdmin: admin
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
            height: mainWeightPanel.visible ? parent.height - mainWeightPanel.height : parent.height
            spacing: 0
            padding: 0
/*
            Rectangle
            {
                id: adminMenuPanel
                width: adminMenuWidth
                height: parent.height
                color: Material.color(Material.Grey, Material.Shade200)

                Loader
                {
                    width: adminMenuWidth
                    source: "Panels/adminMenuPanel.qml"
                }
            }
*/
            Rectangle
            {
                //width: parent.width - adminMenuWidth
                width: parent.width
                height: parent.height
                color: Material.color(Material.Grey, Material.Shade100)

                Rectangle
                {
                    width: parent.width
                    height: parent.height

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
                            //source: "Panels/showcasePanel.qml"
                            source: "Panels/showcasePanel3.qml"
                        }

                        Loader
                        {
                            focus: true
                            source: "Panels/searchPanel.qml"
                        }
                    }
                }
            }
        }
    }
}
