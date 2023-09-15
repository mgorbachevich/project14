import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: confirmationPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: "Title"
    property string messageText: "Message"
    property int dialogSelector: 0
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Rectangle
    {
        anchors.fill: parent
        radius: app.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: app.spacer()
            columnSpacing: app.spacer()
            rowSpacing: app.spacer()
            columns: 1
            rows: 3

            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ confirmationPanel Keys.onPressed ", JSON.stringify(event))
                event.accepted = true;
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_Escape: // Круглая стрелка
                        confirmationPanel.close()
                        break
                    case Qt.Key_Enter: case Qt.Key_Return:
                        app.onConfirmationClicked(dialogSelector)
                        confirmationPanel.close()
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
                Layout.fillWidth: parent
                Layout.preferredHeight: app.buttonSize()
                color: "transparent"

                CardTitleText
                {
                    text: titleText
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                Layout.bottomMargin: app.buttonSize() / 2
                color: "transparent"

                CardText
                {
                    text: messageText
                }
            }

            Row
            {
                spacing: app.spacer() * 4
                Layout.preferredWidth: app.buttonSize() * 3 + spacing
                Layout.preferredHeight: app.buttonSize()
                Layout.column: 0
                Layout.row: 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                RoundTextButton
                {
                    width: app.buttonSize() * 3 / 2
                    height: app.buttonSize()
                    text: qsTr(" ДА ")
                    onClicked:
                    {
                        app.onUserAction();
                        app.onConfirmationClicked(dialogSelector)
                        confirmationPanel.close()
                    }
                 }

                RoundTextButton
                {
                    width: app.buttonSize() * 3 / 2
                    height: app.buttonSize()
                    text: qsTr("НЕТ")
                    onClicked:
                    {
                        app.onUserAction();
                        confirmationPanel.close()
                    }
                }
            }
        }
    }
}

