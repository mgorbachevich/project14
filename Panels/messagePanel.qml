import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
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

    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    enter: Transition { enabled: false }
    exit: Transition { enabled: false }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.color(Material.Grey, Material.Shade100)

        GridLayout
        {
            anchors.fill: parent
            anchors.leftMargin: screenManager.spacer()
            anchors.rightMargin: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0
            focus: true
            Keys.onPressed: (event) =>
            {
                //app.debugLog("@@@@@ messagePanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.onClick();
                switch (event.key)
                {
                    case Qt.Key_F10: // Промотка
                        app.onClicked(26)
                        break
                    default:
                        messagePanel.close()
                        break
                }
            }

            EmptyButton
            {
                Layout.column: 0
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                CardTitleText { text: titleText }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close"
                onClicked:
                {
                    app.onUserAction();
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
                color: "transparent"

                CardText { text: messageText }
            }
        }
    }
}

