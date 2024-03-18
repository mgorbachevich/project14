import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    id:  tablePanel
    color: Material.background
    focus: true
    Keys.onPressed: (event) =>
    {
        app.debugLog("@@@@@ tablePanel Keys.onPressed %1".arg(JSON.stringify(event)))
        event.accepted = true;
        app.clickSound();
        app.onUserAction();
        switch (event.key)
        {
            case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
            case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                app.onNumberClicked(event.text);
                break;
            case Qt.Key_F9: // Ключ
                app.onLockClicked()
                break;
            case Qt.Key_T: // >T<
                app.onTareClicked()
                break
            case Qt.Key_Z: // >0<
                app.onZeroClicked()
                break
            case Qt.Key_Left:
                app.onSwipeMainPage(0)
                break;
            case Qt.Key_Right:
            case Qt.Key_Q:
                app.onSwipeMainPage(2)
                break;
            case Qt.Key_Up:
                if (!tablePanelResultList.atYBeginning) tablePanelResultList.flick(0, Constants.flickVelocity)
                else app.beepSound()
                break;
            case Qt.Key_Down:
                if (!tablePanelResultList.atYEnd) tablePanelResultList.flick(0, -Constants.flickVelocity)
                else app.beepSound()
                break;
            default:
                app.beepSound();
                break;
        }
    }

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === 1)
            {
                app.debugLog("@@@@@ tablePanel onShowMainPage");
                tablePanel.forceActiveFocus()
            }
        }
    }

    Connections // Slot for signal AppManager::showTablePanelTitle:
    {
        target: app
        function onShowTablePanelTitle(value) { tablePanelTitle.text = value }
    }

    Connections // Slot for signal AppManager::showGroupHierarchyRoot:
    {
        target: app
        function onShowGroupHierarchyRoot(value)
        {
            app.debugLog("@@@@@ tablePanel.onShowGroupHierarchyRoot %1".arg(value))
            tablePanelBackButton.icon.source = value ? "../Icons/empty" : "../Icons/arrow_up"
        }
    }

    GridLayout
    {
        id: tablePanelLayout
        anchors.fill: parent
        columnSpacing: screenManager.spacer() / 2
        columns: 3
        rows: 2

        RoundIconButton
        {
            id: tablePanelBackButton
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/empty"
            onClicked: app.onTableBackClicked()
        }

        Text
        {
            id: tablePanelTitle
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            font { pointSize: screenManager.normalFontSize() }
            wrapMode: Text.WordWrap
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: "/"
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: tablePanelLayout.columns
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            Layout.rightMargin: screenManager.spacer() / 2
            color: Material.background

            ListView
            {
                id: tablePanelResultList
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

                model: tablePanelModel
                delegate: Label
                {
                    width: tablePanelResultList.width
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
                        onClicked: app.onTableResultClicked(index)
                    }
                }
            }
        }
    }
}
