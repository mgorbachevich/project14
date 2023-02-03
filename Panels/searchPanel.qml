import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Rectangle
{
    id:  searchPanel
    color: Material.background
    property int filterWidth: width * 0.25

    GridLayout
    {
        id: searchPanelLayout
        anchors.fill: parent
        anchors.leftMargin: Constants.margin
        anchors.rightMargin: Constants.margin
        anchors.topMargin: Constants.margin
        anchors.bottomMargin: Constants.margin * 2
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 3

        Label
        {
            id: searchPanelTitle
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Italic' }
            color: Material.color(Material.Grey, Material.Shade600)
            text: qsTr("Поиск товаров")
        }

        Button
        {
            id: searchPanelOptionsButton
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
            onClicked: app.onSearchOptionsClicked() // AppManager's slot
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: searchPanelResultList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: searchPanelModel
                delegate: Text
                {
                    padding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    text: model.value // Roles::ValueRole
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: app.onSearchResultClicked(index) // AppManager's slot
                    }
                 }
             }
        }

        TextField
        {
            id: searchPanelTextField
            Layout.column: 1
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.preferredWidth: filterWidth
            Layout.bottomMargin: Constants.margin
            font { pointSize: Constants.largeFontSize; family: "Roboto"; styleName:'Bold' }
            placeholderText: "?????"
            inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            onTextEdited: app.onSearchFilterEdited(text) // AppManager's slot
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.preferredWidth: filterWidth
            Layout.fillHeight: parent
            Layout.alignment: Qt.AlignTop
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: searchPanelFilterList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                currentIndex: 0
                highlight: Rectangle { color: Material.primary }
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */

                model: searchFilterModel
                delegate: Text
                {
                    width: parent.width
                    padding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    text: model.value // Roles::ValueRole
                    //color: searchPanelFilterList.currentIndex === index ? "white" : "black"

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            searchPanelFilterList.currentIndex = index
                            app.onSearchFilterClicked(index) // AppManager's slot
                        }
                    }
                }
            }
        }
    }
}
