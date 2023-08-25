import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    id:  showcasePanel
    color: Material.background
    property int imageSize: (height - Constants.margin * 3 + 1) / 2

    focus: true
    Keys.onPressed: (event) =>
    {
        console.debug("@@@@@ showcasePanel Keys.onPressed ", JSON.stringify(event));
        event.accepted = true;
        app.onUserAction(); // AppManager's slot
        switch (event.key)
        {
            case Qt.Key_F9: // Ключ
                app.onLockClicked()
                break;
            case Qt.Key_T: // >T<
                app.onTareClicked()
                break
            case Qt.Key_Z: // >0<
                app.onZeroClicked()
                break
            case Qt.Key_Right:
                app.onSwipeMainPage(1)
                break;
            case Qt.Key_Q:
                app.onSwipeMainPage(2)
                break;
            case Qt.Key_Up:
                if (!showcasePanelGrid.atYBeginning) showcasePanelGrid.flick(0, Constants.flickVelocity)
                else app.onBeep()
                break;
            case Qt.Key_Down:
                if (!showcasePanelGrid.atYEnd) showcasePanelGrid.flick(0, -Constants.flickVelocity)
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
            if (index === 0)
            {
                console.debug("@@@@@ showcasePanel onShowMainPage");
                showcasePanel.forceActiveFocus()
            }
        }
    }

    GridView
    {
        id: showcasePanelGrid
        anchors.fill: parent
        anchors.rightMargin: Constants.margin
        leftMargin: Constants.margin
        topMargin: Constants.margin
        bottomMargin: Constants.margin * 2
        clip: true
        cellWidth: showcasePanel.imageSize + Constants.margin
        cellHeight: showcasePanel.imageSize + Constants.margin

        ScrollBar.vertical: ScrollBar
        {
            width: Constants.margin
            background: Rectangle { color: "transparent" }
            policy: ScrollBar.AlwaysOn
        }

        model: showcasePanelModel
        delegate: Image
        {
            width: showcasePanel.imageSize
            height: showcasePanel.imageSize
            source: model.value  // Roles::ValueRole
            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    app.onUserAction(); // AppManager's slot
                    app.onShowcaseClicked(index) // AppManager's slot
                }
            }
        }
    }
}


