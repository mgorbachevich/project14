import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: settingPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    property string panelTitle: ""
    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    Connections // Slot for signal AppManager::previousSettings
    {
        target: app
        function onPreviousSettings() { settingPanel.close() }
    }

    GridLayout
    {
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        focus: true

        Keys.onPressed: (event) =>
        {
            //app.debugLog("@@@@@ settingPanel Keys.onPressed %1".arg(JSON.stringify(event)))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!settingPanelList.atYBeginning) settingPanelList.flick(0, screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!settingPanelList.atYEnd) settingPanelList.flick(0, -screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    settingPanel.close()
                    break
                case Qt.Key_F10: // Промотка
                    app.onRewind()
                    break
                default:
                    app.beepSound();
                    break
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.preferredWidth: screenManager.buttonSize() + screenManager.spacer() * 2
            color: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: screenManager.buttonSize() + screenManager.spacer() * 2
            color: "transparent"

            CardTitleText { text: panelTitle }
        }

        Rectangle
        {
            Layout.column: 2
            Layout.row: 0
            Layout.preferredWidth: screenManager.buttonSize() + screenManager.spacer() * 2
            color: "transparent"

            RoundIconButton
            {
                icon.source: "../Icons/close"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: app.onSettingsPanelCloseClicked()
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 3
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: settingPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */
                model: settingsPanelModel
                delegate: Row
                {
                    leftPadding: screenManager.spacer() * 2
                    topPadding: screenManager.spacer()
                    rightPadding: screenManager.spacer()
                    bottomPadding: screenManager.spacer()
                    spacing: screenManager.spacer() * 2

                    Label
                    {
                        width: (settingPanelList.width - screenManager.spacer() * 5) * 0.6
                        font { pointSize: screenManager.normalFontSize() }
                        wrapMode: Text.WordWrap
                        text: model.name
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingsItemClicked(index)
                        }
                    }

                    Label
                    {
                        width: (settingPanelList.width - screenManager.spacer() * 5) * 0.4
                        font { pointSize: screenManager.normalFontSize() }
                        wrapMode: Text.WordWrap
                        text: model.value
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingsItemClicked(index)
                        }
                    }
                }
            }
        }
    }
}

