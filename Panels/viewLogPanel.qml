import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: viewLogPanel
    //objectName: "viewLogPanel"
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columnSpacing: screenManager.spacer()
        rowSpacing: screenManager.spacer()
        columns: 2
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ viewLogPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                    viewLogPanel.close()
                    break
                case Qt.Key_Enter: case Qt.Key_Return:
                    viewLogPanel.close()
                    break
                case Qt.Key_Up:
                    if (!viewLogPanelList.atYBeginning) viewLogPanelList.flick(0, Constants.flickVelocity)
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!viewLogPanelList.atYEnd) viewLogPanelList.flick(0, -Constants.flickVelocity)
                    else app.beepSound()
                    break;
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
            color: Material.background

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: screenManager.normalFontSize() }
                color: Material.color(Material.Grey, Material.Shade600)
                wrapMode: Text.WordWrap
                text: qsTr("Лог")
            }
        }

        RoundIconButton
        {
            Layout.column: 1
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            icon.source: "../Icons/close_black_48"
            onClicked:
            {
                app.onUserAction();
                viewLogPanel.close()
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: viewLogPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()

                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.spacer()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: viewLogPanelModel
                delegate: Text
                {
                    leftPadding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.Grey, Material.Shade900)
                    text: model.value
                }
            }
        }
    }
}

