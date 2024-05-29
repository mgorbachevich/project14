import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: viewLogPanel
    //objectName: "viewLogPanel"
    padding : 0
    Material.background: Material.color(Material.Grey, Material.Shade100)
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    Connections // Slot for signal AppManager::closeLogView
    {
        target: app
        function onCloseLogView() { viewLogPanel.close() }
    }

    GridLayout
    {
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0

        focus: true
        Keys.onPressed: (event) =>
        {
            //app.debugLog("@@@@@ viewLogPanel Keys.onPressed %1".arg(JSON.stringify(event)))
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
                    if (!viewLogPanelList.atYBeginning) viewLogPanelList.flick(0, screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!viewLogPanelList.atYEnd) viewLogPanelList.flick(0, -screenManager.flickVelocity())
                    else app.beepSound()
                    break;
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
                icon.source: "../Icons/delete"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked:
                {
                    app.onUserAction();
                    app.clearLog()
                }
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            color: Material.color(Material.Grey, Material.Shade100)

            CardTitleText { text: qsTr("Лог") }
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
                onClicked:
                {
                    app.onUserAction();
                    viewLogPanel.close()
                }
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
                id: viewLogPanelList
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
                model: viewLogPanelModel
                delegate: Text
                {
                    leftPadding: screenManager.spacer() * 2
                    rightPadding: screenManager.spacer() * 2
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.Grey, Material.Shade900)
                    text: model.value
                }
            }
        }
    }
}

