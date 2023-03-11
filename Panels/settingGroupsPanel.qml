import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: settingGroupsPanel
    objectName: "settingGroupsPanel"
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 2
        /*
        focus: true
        Keys.onPressed: (event) => {}
        */

        Button
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/empty_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            Text
            {
                id: settingGroupsPanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: qsTr("Настройки")
            }
        }

        Button
        {
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/close_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
            onClicked: settingGroupsPanel.close()
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

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: settingGroupsPanelModel
                delegate: Row
                {
                    padding: Constants.margin
                    spacing: Constants.margin

                    Text
                    {
                        font { pointSize: Constants.normalFontSize; family: "Roboto" }
                        padding: Constants.margin
                        text: model.value

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onSettingGroupClicked(index) // AppManager's slot
                        }
                    }
                }
            }
        }
    }
}

