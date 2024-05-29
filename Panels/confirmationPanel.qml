import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
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
    property int confirmSelector: 0
    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.color(Material.Grey, Material.Shade100)

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0
            focus: true
            Keys.onPressed: (event) =>
            {
                //app.debugLog("@@@@@ confirmationPanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_F10: // Промотка
                        app.onRewind()
                        break
                    case Qt.Key_Escape: // Круглая стрелка
                        confirmationPanel.close()
                        break
                    case Qt.Key_Enter: case Qt.Key_Return:
                        app.onConfirmationClicked(confirmSelector)
                        confirmationPanel.close()
                        break
                    default:
                        app.beepSound();
                        break
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                CardTitleText { text: titleText }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                color: "transparent"

                CardText { text: messageText }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 2
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                Row
                {
                    spacing: screenManager.spacer() * 4
                    width: screenManager.buttonSize() * 3 + spacing
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    RoundTextButton
                    {
                        width: screenManager.buttonSize() * 3 / 2
                        text: qsTr("ДА")
                        onClicked:
                        {
                            app.onConfirmationClicked(confirmSelector)
                            confirmationPanel.close()
                        }
                    }

                    RoundTextButton
                    {
                        width: screenManager.buttonSize() * 3 / 2
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
}

