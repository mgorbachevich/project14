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
    property int spacer: screenManager.spacer() / 2
    property int imageSize: (width - screenManager.scrollBarWidth() - spacer * 2) / screenManager.showcaseRowImages() - spacer

    focus: true
    Keys.onPressed: (event) =>
    {
        console.debug("@@@@@ showcasePanel Keys.onPressed ", JSON.stringify(event));
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
            case Qt.Key_Right:
                app.onSwipeMainPage(1)
                break;
            case Qt.Key_Q:
                app.onSwipeMainPage(2)
                break;
            case Qt.Key_Up:
                if (!showcasePanelGrid.atYBeginning) showcasePanelGrid.flick(0, Constants.flickVelocity)
                else app.beepSound()
                break;
            case Qt.Key_Down:
                if (!showcasePanelGrid.atYEnd) showcasePanelGrid.flick(0, -Constants.flickVelocity)
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
        anchors.leftMargin: spacer
        anchors.rightMargin: spacer
        anchors.topMargin: spacer
        anchors.bottomMargin: spacer
        clip: true
        cellWidth: imageSize + spacer
        cellHeight: imageSize + spacer

        ScrollBar.vertical: ScrollBar
        {
            width: screenManager.scrollBarWidth()
            background: Rectangle { color: "transparent" }
            policy: ScrollBar.AlwaysOn
        }
        /*
        model: showcasePanelModel
        delegate: Image
        {
            width: showcasePanelGrid.cellWidth - spacer
            height: showcasePanelGrid.cellHeight - spacer
            fillMode: Image.PreserveAspectFit
            source: model.value  // Roles::ValueRole
            MouseArea
            {
                anchors.fill: parent
                onClicked: app.onShowcaseClicked(index)
            }
        }
        */
        model: showcasePanelModel
        delegate: Label
        {
            width: showcasePanelGrid.cellWidth - spacer
            height: showcasePanelGrid.cellHeight - spacer
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            text: model.first
            background: Image
            {
                fillMode: Image.PreserveAspectFit
                source: model.second
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: app.onShowcaseClicked(index)
            }
        }
    }
}



