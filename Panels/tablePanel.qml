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
        console.debug("@@@@@ tablePanel Keys.onPressed ", JSON.stringify(event));
        event.accepted = true;
        app.onUserAction(); // AppManager's slot
        switch (event.key)
        {
            case Qt.Key_F9: // Ключ
                app.onLockClicked()
                break;
            case Qt.Key_T: // >T<
                app.onWeightParamClicked(1);
                break;
            case Qt.Key_Z: // >0<
                app.onWeightParamClicked(0);
                break;
            case Qt.Key_Left:
                app.onSwipeMainPage(0)
                break;
            case Qt.Key_Right:
            case Qt.Key_Q:
                app.onSwipeMainPage(2)
                break;
            case Qt.Key_Up:
                if (!tablePanelResultList.atYBeginning) tablePanelResultList.flick(0, Constants.flickVelocity)
                else app.onBeep()
                break;
            case Qt.Key_Down:
                if (!tablePanelResultList.atYEnd) tablePanelResultList.flick(0, -Constants.flickVelocity)
                else app.onBeep()
                break;
            default:
                app.onBeep();
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
                console.debug("@@@@@ tablePanel onShowMainPage");
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
            console.debug("@@@@@ tablePanel.onShowGroupHierarchyRoot " , value);
            tablePanelBackButton.icon.source = value ? "../Icons/empty_48" : "../Icons/arrow_up_black_48"
        }
    }

    GridLayout
    {
        id: tablePanelLayout
        anchors.fill: parent
        anchors.leftMargin: Constants.margin
        anchors.rightMargin: Constants.margin
        anchors.topMargin: Constants.margin
        anchors.bottomMargin: Constants.margin * 2
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 2

        RoundIconButton
        {
            id: tablePanelBackButton
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/empty_48"
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onTableBackClicked() // AppManager's slot
            }
        }

        Text
        {
            id: tablePanelTitle
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            font { pointSize: Constants.normalFontSize }
            wrapMode: Text.WordWrap
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: "/"
        }

        RoundIconButton
        {
            id: tablePanelOptionsButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            icon.source: "../Icons/settings_black_48"
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onTableOptionsClicked() // AppManager's slot
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 3
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.background

            ListView
            {
                id: tablePanelResultList
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

                model: tablePanelModel
                delegate: Label
                {
                    width: tablePanelResultList.width
                    font { pointSize: Constants.normalFontSize }
                    padding: Constants.margin
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
                        onClicked:
                        {
                            app.onUserAction(); // AppManager's slot
                            app.onTableResultClicked(index) // AppManager's slot
                        }
                    }
                }
            }
        }
    }
}
