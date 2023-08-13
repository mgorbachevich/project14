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
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 2
        rows: 2

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ viewLogPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.onUserAction(); // AppManager's slot
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                    viewLogPanel.close()
                    break
                case Qt.Key_Enter:
                    viewLogPanel.close()
                    break
                case Qt.Key_Up:
                    if (!viewLogPanelList.atYBeginning) viewLogPanelList.flick(0, Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Down:
                    if (!viewLogPanelList.atYEnd) viewLogPanelList.flick(0, -Constants.flickVelocity)
                    else app.onBeep()
                    break;
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
            color: Material.background

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize }
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
                app.onUserAction(); // AppManager's slot
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
                onFlickStarted: app.onUserAction() // AppManager's slot

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: viewLogPanelModel
                delegate: Text
                {
                    leftPadding: Constants.margin
                    font { pointSize: Constants.normalFontSize }
                    color: Material.color(Material.Grey, Material.Shade900)
                    text: model.value
                }
            }
        }
    }
}

