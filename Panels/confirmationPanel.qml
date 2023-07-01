import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

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
        radius: Constants.margin
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: Constants.margin
            columnSpacing: Constants.margin
            rowSpacing: Constants.margin
            columns: 1
            rows: 3

            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ confirmationPanel Keys.onPressed ", JSON.stringify(event))
                event.accepted = true;
                switch (event.key)
                {
                    case Qt.Key_Escape: // Круглая стрелка
                        confirmationPanel.close()
                        break
                    case Qt.Key_Enter:
                        app.onConfirmationClicked(dialogSelector)
                        confirmationPanel.close()
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
                Layout.fillWidth: parent
                Layout.preferredHeight: Constants.buttonSize
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
                Layout.bottomMargin: Constants.buttonSize / 2
                color: "transparent"

                CardText
                {
                    text: messageText
                }
            }

            Row
            {
                spacing: Constants.margin * 4
                Layout.preferredWidth: Constants.buttonSize * 3 + spacing
                Layout.preferredHeight: Constants.buttonSize
                Layout.column: 0
                Layout.row: 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                RoundTextButton
                {
                    width: Constants.buttonSize * 3 / 2
                    height: Constants.buttonSize
                    text: qsTr(" ДА ")
                    onClicked:
                    {
                        app.onConfirmationClicked(dialogSelector) // AppManager's slot
                        confirmationPanel.close()
                    }
                 }

                RoundTextButton
                {
                    width: Constants.buttonSize * 3 / 2
                    height: Constants.buttonSize
                    text: qsTr("НЕТ")
                    onClicked: confirmationPanel.close()
                }
            }
        }
    }
}

