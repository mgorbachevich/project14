import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: editUsersPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        app.onEditUsersPanelClose()
        app.onPopupOpened(false)
    }

    GridLayout
    {
        id: editUsersPanelLayout
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        focus: true

        Keys.onPressed: (event) =>
        {
            //app.debugLog("@@@@@ editUsersPanel Keys.onPressed %1".arg(JSON.stringify(event)))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!editUsersPanelList.atYBeginning) editUsersPanelList.flick(0, screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!editUsersPanelList.atYEnd) editUsersPanelList.flick(0, -screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    editUsersPanel.close()
                    break
                case Qt.Key_F10: // Промотка
                    app.onRewind()
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
            Layout.preferredWidth: screenManager.buttonSize() + screenManager.spacer() * 2
            Layout.preferredHeight: screenManager.buttonSize() + screenManager.spacer() * 2
            color: Material.color(Material.Grey, Material.Shade100)

            RoundIconButton
            {
                icon.source: "../Icons/plus"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: app.onAddUserClicked()
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            color: Material.color(Material.Grey, Material.Shade100)

            CardTitleText { text: qsTr("Пользователи") }
        }

        Rectangle
        {
            Layout.column: 2
            Layout.row: 0
            Layout.preferredWidth: screenManager.buttonSize() + screenManager.spacer() * 2
            Layout.preferredHeight: screenManager.buttonSize() + screenManager.spacer() * 2
            color: Material.color(Material.Grey, Material.Shade100)

            RoundIconButton
            {
                icon.source: "../Icons/close"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: editUsersPanel.close()
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 3
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: editUsersPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */
                model: editUsersPanelModel
                delegate: Row
                {
                    leftPadding: screenManager.spacer() * 2
                    topPadding: screenManager.spacer()
                    rightPadding: screenManager.spacer()
                    bottomPadding: screenManager.spacer()
                    spacing: screenManager.spacer() * 2

                    Label
                    {
                        width: (editUsersPanelList.width - screenManager.spacer() * 5) * 0.08
                        font { pointSize: screenManager.normalFontSize() }
                        color: Material.color(Material.BlueGrey, Material.Shade900)
                        horizontalAlignment: Text.AlignRight
                        text: model.code
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onEditUsersPanelClicked(index)
                        }
                    }

                    Label
                    {
                        width: (editUsersPanelList.width - screenManager.spacer() * 5) * 0.92
                        font { pointSize: screenManager.normalFontSize() }
                        color: Material.color(Material.BlueGrey, Material.Shade900)
                        horizontalAlignment: Text.AlignLeft
                        text: model.name
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onEditUsersPanelClicked(index)
                        }
                    }
                }
            }
        }
    }
}
