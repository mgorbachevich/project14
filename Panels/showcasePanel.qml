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
    property int imageSize: (height - app.spacer() * 3 / 2 + 1) / 2

    focus: true
    Keys.onPressed: (event) =>
    {
        console.debug("@@@@@ showcasePanel Keys.onPressed ", JSON.stringify(event));
        event.accepted = true;
        app.onUserAction();
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
                else app.beep()
                break;
            case Qt.Key_Down:
                if (!showcasePanelGrid.atYEnd) showcasePanelGrid.flick(0, -Constants.flickVelocity)
                else app.beep()
                break;
            default:
                app.beep();
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
        anchors.rightMargin: app.spacer() / 2
        anchors.leftMargin: app.spacer() / 2
        anchors.topMargin: app.spacer() / 2
        anchors.bottomMargin: app.spacer() / 2
        clip: true
        cellWidth: showcasePanel.imageSize + app.spacer() / 2
        cellHeight: showcasePanel.imageSize + app.spacer() / 2

        ScrollBar.vertical: ScrollBar
        {
            width: app.scrollBarWidth()
            background: Rectangle { color: "transparent" }
            policy: ScrollBar.AlwaysOn
        }

        model: showcasePanelModel
        delegate: Image
        {
            width: showcasePanel.imageSize
            height: showcasePanel.imageSize
            fillMode: Image.PreserveAspectFit
            source: model.value  // Roles::ValueRole
            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    app.onUserAction();
                    app.onShowcaseClicked(index)
                }
            }
        }
    }
}


