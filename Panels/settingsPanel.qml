import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
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
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::closeSettings
    {
        target: app
        function onCloseSettings() { settingPanel.close() }
    }

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columnSpacing: 0
        rowSpacing: 0
        columns: 3
        rows: 2
        focus: true

        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ settingPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!settingPanelList.atYBeginning) settingPanelList.flick(0, Constants.flickVelocity)
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!settingPanelList.atYEnd) settingPanelList.flick(0, -Constants.flickVelocity)
                    else app.beepSound()
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    settingPanel.close()
                    break
                default:
                    app.beepSound();
                    break
            }
        }

        EmptyButton
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            color: "transparent"

            CardTitleText { text: panelTitle }
        }

        RoundIconButton
        {
            Layout.column: 2
            Layout.row: 0
            icon.source: "../Icons/close_black_48"
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            onClicked: app.onSettingsPanelCloseClicked()
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 3
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            Layout.rightMargin: screenManager.spacer() / 2
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: settingPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()

                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: settingsPanelModel
                delegate: Row
                {
                    Label
                    {
                        width: settingPanelList.width * 2 / 3
                        padding: screenManager.spacer()
                        font { pointSize: screenManager.normalFontSize() }
                        wrapMode: Text.WordWrap
                        text: model.first // Roles::FirstRole

                        background: Rectangle
                        {
                            color: index % 2 === 0 ? Material.color(Material.Grey, Material.Shade50) :
                                                     Material.color(Material.Grey, Material.Shade200)
                        }
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingsItemClicked(index)
                        }
                    }

                    Label
                    {
                        width: settingPanelList.width / 3
                        font { pointSize: screenManager.normalFontSize() }
                        padding: screenManager.spacer()
                        wrapMode: Text.WordWrap
                        text: model.second // Roles::SecondRole

                        background: Rectangle
                        {
                            color: index % 2 === 0 ? Material.color(Material.Grey, Material.Shade50) :
                                                     Material.color(Material.Grey, Material.Shade200)
                        }
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

