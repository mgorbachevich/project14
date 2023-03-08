import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: confirmationPanel
    objectName: "confirmationPanel"
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    property string titleText: "Title"
    property string messageText: "Message"
    property int dialogSelector: 0

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 2
        rows: 3
        /*
        focus: true
        Keys.onPressed: (event) => {}
        */

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            Text
            {
                id: cofirmationPanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: titleText
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: "transparent"
            //color: Material.color(Material.Grey, Material.Shade50)

            Text
            {
                id: cofirmationPanelText
                anchors.horizontalCenter: parent.horizontalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Regular' }
                wrapMode: Text.WordWrap
                text: messageText
            }
        }

        Button
        {
            Layout.column: 0
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignCenter
            text: qsTr(" ДА ")
            Material.background: Material.primary
            onClicked:
            {
                app.onConfirmationClicked(dialogSelector) // AppManager's slot
                confirmationPanel.close()
            }
        }

        Button
        {
            Layout.column: 1
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignCenter
            text: qsTr(" НЕТ ")
            Material.background: Material.primary
            onClicked: confirmationPanel.close()
        }
    }
}

