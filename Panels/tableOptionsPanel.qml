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
        anchors.margins: Constants.margin
        rowSpacing: Constants.margin
        columnSpacing: 0
        columns: 4
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ tableOptionsPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.onUserAction(); // AppManager's slot
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                case Qt.Key_Enter:
                    tableOptionsPanel.close()
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
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: qsTr("Настройки таблицы")
            }
        }

        Button
        {
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon.source: "../Icons/close_black_48"
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                tableOptionsPanel.close()
            }
        }

        Column
        {
            Layout.column: 1
            Layout.row: 1
            Layout.maximumWidth: tableOptionsPanel.width / 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            leftPadding: Constants.margin
            rightPadding: Constants.margin

            Text
            {
                height: Constants.margin * 3
                leftPadding: Constants.margin
                text: qsTr("Сортировать")
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По коду");
                onClicked: app.onUserAction(); // AppManager's slot
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По штрих-коду")
                onClicked: app.onUserAction(); // AppManager's slot
            }

            RadioButton
            {
                height: Constants.margin * 4
                text: qsTr("По наименованию")
                checked: true
                onClicked: app.onUserAction(); // AppManager's slot
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

            Text
            {
                height: Constants.margin * 3
                leftPadding: Constants.margin
                text: qsTr("Показывать")
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Код")
                onClicked: app.onUserAction(); // AppManager's slot
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Штрих-код")
                onClicked: app.onUserAction(); // AppManager's slot
            }

            CheckBox
            {
                height: Constants.margin * 4
                text: qsTr("Наименование")
                checked: true
                onClicked: app.onUserAction(); // AppManager's slot
            }
        }
    }
}


