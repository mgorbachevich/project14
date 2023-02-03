import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Rectangle
{
    id:  tablePanel
    color: Material.background

    Connections // Slot for signal AppManager::showTablePanelTitle:
    {
        target: app
        function onShowTablePanelTitle(value) { tablePanelTitle.text = value }
    }

    Connections // Slot for signal AppManager::showGroupHierarchyRoot:
    {
        target: app
        function onShowGroupHierarchyRoot(value)
        {
            console.debug("@@@@@ tablePanel.onShowGroupHierarchyRoot " , value);
            tablePanelBackButton.icon.source = value ? "../Icons/empty_48" : "../Icons/arrow_up_black_48"
        }
    }

    GridLayout
    {
        id: tablePanelLayout
        anchors.fill: parent
        anchors.leftMargin: Constants.margin
        anchors.rightMargin: Constants.margin
        anchors.topMargin: Constants.margin
        anchors.bottomMargin: Constants.margin * 2
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 2

        Button
        {
            id: tablePanelBackButton
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
            Material.background: Material.primary
            onClicked: app.onTableBackClicked() // AppManager's slot
        }

        TextArea
        {
            id: tablePanelTitle
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Italic' }
            wrapMode: TextArea.WordWrap
            background: Rectangle { color: "transparent" }
            text: "/"
        }

        Button
        {
            id: tablePanelOptionsButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/settings_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
            onClicked: app.onTableOptionsClicked() // AppManager's slot
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
                id: tablePanelResultList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: tablePanelModel
                delegate: Text
                {
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    padding: Constants.margin
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: app.onTableResultClicked(index) // AppManager's slot
                    }
                }
            }
        }
    }
}
