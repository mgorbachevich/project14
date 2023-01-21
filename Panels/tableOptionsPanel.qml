import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants

Popup
{
    id: tableOptionsPanel
    padding : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    modal: true
    dim: true

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: Constants.margin
        rowSpacing: Constants.margin
        columnSpacing: 0
        columns: 4
        rows: 2

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
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            TextArea
            {
                id: messagePanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: TextArea.WordWrap
                background: Rectangle  { color: "transparent"  }
                text: qsTr("Настройки таблицы")
            }
        }

        Button
        {
            id: messagePanelCloseButton
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/close_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            onClicked: tableOptionsPanel.close()
        }

        Column
        {
            Layout.column: 1
            Layout.row: 1
            Layout.maximumWidth: tableOptionsPanel.width / 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            leftPadding: Constants.margin
            rightPadding: Constants.margin

            Label
            {
                height: Constants.margin * 3
                leftPadding: Constants.margin
                text: qsTr("Сортировать")
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По коду");
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По штрих-коду")
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По наименованию")
                checked: true
            }
        }

        Column
        {
            Layout.column: 2
            Layout.row: 1
            Layout.maximumWidth: tableOptionsPanel.width / 2
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            leftPadding: Constants.margin
            rightPadding: Constants.margin

            Label
            {
                height: Constants.margin * 3
                leftPadding: Constants.margin
                text: qsTr("Показывать")
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Код")
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Штрих-код")
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Наименование")
                checked: true
            }
        }
    }
}


