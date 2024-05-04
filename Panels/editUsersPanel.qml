import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
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
        anchors.margins: screenManager.spacer()
        columnSpacing: screenManager.spacer()
        columns: 3
        rows: 2
        focus: true

        Keys.onPressed: (event) =>
        {
            app.debugLog("@@@@@ editUsersPanel Keys.onPressed %1".arg(JSON.stringify(event)))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Up:
                    if (!editUsersPanelList.atYBeginning) editUsersPanelList.flick(0, Constants.flickVelocity)
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!editUsersPanelList.atYEnd) editUsersPanelList.flick(0, -Constants.flickVelocity)
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

        RoundIconButton
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/plus"
            onClicked: app.onAddUserClicked()
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            color: "transparent"

            CardTitleText { text: "Пользователи" }
        }

        RoundIconButton
        {
            Layout.column: 2
            Layout.row: 0
            icon.source: "../Icons/close"
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            onClicked: editUsersPanel.close()
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: editUsersPanelLayout.columns
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            Layout.rightMargin: screenManager.spacer() / 2
            color: Material.background

            ListView
            {
                id: editUsersPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()

                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: editUsersPanelModel
                delegate: Label
                {
                    width: editUsersPanelList.width
                    font { pointSize: screenManager.normalFontSize() }
                    padding: screenManager.spacer()
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    background: Rectangle
                    {
                        color: index % 2 === 0 ? Material.color(Material.Grey, Material.Shade50) :
                                                 Material.color(Material.Grey, Material.Shade200)
                    }
                    text: model.value // Roles::ValueRole
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
