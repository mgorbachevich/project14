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

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ settingPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!settingPanelList.atYBeginning) settingPanelList.flick(0, Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Down:
                    if (!settingPanelList.atYEnd) settingPanelList.flick(0, -Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    settingPanel.close()
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
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            CardTitleText
            {
                text: panelTitle
            }
        }

        RoundIconButton
        {
            Layout.column: 2
            Layout.row: 0
            icon.source: "../Icons/close_black_48"
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            onClicked: settingPanel.close()
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

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: settingsPanelModel
                delegate: Row
                {
                    padding: Constants.margin
                    spacing: Constants.margin

                    Text
                    {
                        width: settingPanelList.width / 2 - Constants.margin * 4
                        font { pointSize: Constants.normalFontSize }
                        wrapMode: Text.WordWrap
                        text: model.first // Roles::FirstRole

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingsItemClicked(index) // AppManager's slot
                        }
                    }

                    Text
                    {
                        width: settingPanelList.width / 2
                        font { pointSize: Constants.normalFontSize }
                        wrapMode: Text.WordWrap
                        text: model.second // Roles::SecondRole

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingsItemClicked(index) // AppManager's slot
                        }
                    }
                }
            }
        }
    }
}

