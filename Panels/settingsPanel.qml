import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: settingPanel
    objectName: "settingPanel"
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    property string panelTitle: ""

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
                id: settingPanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: panelTitle
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
                        font { pointSize: Constants.normalFontSize; family: "Roboto" }
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
                        font { pointSize: Constants.normalFontSize; family: "Roboto" }
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
            /*
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

                model: settingPanelModel
                delegate: Text
                {
                    leftPadding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    text: model.value // Roles::ValueRole
                }
            }
            */
        }
    }
}

