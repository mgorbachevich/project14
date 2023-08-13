import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: messagePanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: "Title"
    property string messageText: "Message"
    property bool buttonVisibility: true
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::hideMessageBox
    {
        target: app
        function onHideMessageBox() { messagePanel.close() }
    }

    Rectangle
    {
        anchors.fill: parent
        radius: Constants.margin
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

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
                console.debug("@@@@@ messagePanel Keys.onPressed ", JSON.stringify(event))
                event.accepted = true;
                app.onUserAction(); // AppManager's slot
                messagePanel.close()
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
                Layout.fillWidth: parent
                Layout.preferredHeight: Constants.buttonSize
                color: "transparent"

                CardTitleText
                {
                    text: titleText
                }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close_black_48"
                visible: buttonVisibility
                onClicked:
                {
                    app.onUserAction(); // AppManager's slot
                    messagePanel.close()
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                Layout.bottomMargin: Constants.buttonSize * 3 / 4
                color: "transparent"

                CardText
                {
                    text: messageText
                }
            }
        }
    }
}

