import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    Material.theme: Material.Light

    id: tableOptionsPanel
    //objectName: "tableOptionsPanel"
    padding : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    modal: true
    dim: true
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: app.spacer()
        rowSpacing: app.spacer()
        columnSpacing: 0
        columns: 4
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ tableOptionsPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Escape: case Qt.Key_Enter: case Qt.Key_Return:
                    tableOptionsPanel.close()
                    break
                default:
                    app.beep();
                    break
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            Layout.preferredWidth: app.buttonSize()
            Layout.preferredHeight: app.buttonSize()
            color: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: app.buttonSize()
            color: "transparent"

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: app.normalFontSize(); styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: qsTr("Настройки таблицы")
            }
        }

        Button
        {
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            Layout.preferredWidth: app.buttonSize()
            Layout.preferredHeight: app.buttonSize()
            icon.source: "../Icons/close_black_48"
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            onClicked:
            {
                app.onUserAction();
                tableOptionsPanel.close()
            }
        }

        Column
        {
            Layout.column: 1
            Layout.row: 1
            Layout.maximumWidth: tableOptionsPanel.width / 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            leftPadding: app.spacer()
            rightPadding: app.spacer()

            Text
            {
                height: app.spacer() * 3
                leftPadding: app.spacer()
                text: qsTr("Сортировать")
            }

            RadioButton
            {
                height: app.spacer() * 4
                text: qsTr("По коду");
                onClicked: app.onUserAction();
            }

            RadioButton
            {
                height: app.spacer() * 4
                text: qsTr("По штрих-коду")
                onClicked: app.onUserAction();
            }

            RadioButton
            {
                height: app.spacer() * 4
                text: qsTr("По наименованию")
                checked: true
                onClicked: app.onUserAction();
            }
        }

        Column
        {
            Layout.column: 2
            Layout.row: 1
            Layout.maximumWidth: tableOptionsPanel.width / 2
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            leftPadding: app.spacer()
            rightPadding: app.spacer()

            Text
            {
                height: app.spacer() * 3
                leftPadding: app.spacer()
                text: qsTr("Показывать")
            }

            CheckBox
            {
                height: app.spacer() * 4
                text: qsTr("Код")
                onClicked: app.onUserAction();
            }

            CheckBox
            {
                height: app.spacer() * 4
                text: qsTr("Штрих-код")
                onClicked: app.onUserAction();
            }

            CheckBox
            {
                height: app.spacer() * 4
                text: qsTr("Наименование")
                checked: true
                onClicked: app.onUserAction();
            }
        }
    }
}


