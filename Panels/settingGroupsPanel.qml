import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: settingGroupsPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: app.spacer()
        columnSpacing: app.spacer()
        rowSpacing: app.spacer()
        columns: 3
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ settingGroupsPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.onUserAction(); // AppManager's slot
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!settingGroupsPanelList.atYBeginning) settingGroupsPanelList.flick(0, Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Down:
                    if (!settingGroupsPanelList.atYEnd) settingGroupsPanelList.flick(0, -Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    settingGroupsPanel.close()
                    break
                default:
                    app.onBeep();
                    break
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.preferredWidth: app.buttonSize()
            Layout.preferredHeight: app.buttonSize()
            color: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: app.buttonSize()
            color: "transparent"

            CardTitleText
            {
                text: qsTr("Настройки")
            }
        }

        RoundIconButton
        {
            Layout.column: 2
            Layout.row: 0
            icon.source: "../Icons/close_black_48"
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                settingGroupsPanel.close()
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
                id: settingGroupsPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction() // AppManager's slot

                ScrollBar.vertical: ScrollBar
                {
                    width: app.spacer()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: settingGroupsPanelModel
                delegate: Row
                {
                    padding: app.spacer()
                    spacing: app.spacer()

                    Text
                    {
                        font { pointSize: app.normalFontSize() }
                        padding: app.spacer()
                        text: model.value

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                app.onUserAction(); // AppManager's slot
                                app.onSettingGroupClicked(index) // AppManager's slot
                            }
                        }
                    }
                }
            }
        }
    }
}

