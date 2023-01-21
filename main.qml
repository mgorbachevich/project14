import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "constants.js" as Constants
import RegisteredTypes 1.0

ApplicationWindow
{
    Material.theme: Material.Light
    Material.background: Material.color(Material.Grey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    Material.accent: Material.Orange
    Material.primary: Material.color(Material.Indigo, Material.Shade100)

    id: mainWindow
    width: Constants.mainWindowWidth
    height:  Constants.mainWindowHeight
    title: qsTr("Project14")
    color: Material.background
    visible: true
    property int popupX: (mainWindow.width - mainWindow.width * 4 / 5) / 2
    property int popupY: mainWindow.height / 4 + Constants.margin
    property int popupWidth: mainWindow.width * 4 / 5
    property int popupHeight: mainWindow.height * 2 / 3

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

    Connections // Slot for signal AppManager::showAuthorizationPanel:
    {
        target: app
        function onShowAuthorizationPanel()
        {
            console.debug("@@@@@ mainWindow.onShowAuthorizationPanel");
            Qt.createComponent("Panels/authorizationPanel.qml").createObject(mainWindow,
                                                                       {
                                                                           x: 0,
                                                                           y: 0,
                                                                           width: mainWindow.width,
                                                                           height: mainWindow.height
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
        spacing: 0

        Loader
        {
            id: mainWeightPanel
            width: mainWindow.width
            source: "Panels/weightPanel.qml"
        }

        Rectangle
        {
            width: mainWindow.width
            height: mainWindow.height - mainWeightPanel.height

            SwipeView
            {
                id: mainSwipeView
                anchors.fill: parent
                currentIndex: 0

                Loader { source: "Panels/showcasePanel.qml" }
                Loader { source: "Panels/tablePanel.qml" }
                Loader { source: "Panels/searchPanel.qml" }
            }
        }
    }

    Rectangle
    {
        height: 16
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        PageIndicator
        {
            anchors.horizontalCenter: parent.horizontalCenter
            count: mainSwipeView.count
            currentIndex: mainSwipeView.currentIndex
         }
    }
}
