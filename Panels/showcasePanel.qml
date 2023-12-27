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
    property int imageSpacer: screenManager.spacer() / 2
    property int imageSize: (width - screenManager.scrollBarWidth() - imageSpacer * 2) / 5 - imageSpacer

    focus: true
    Keys.onPressed: (event) =>
    {
        console.debug("@@@@@ showcasePanel Keys.onPressed ", JSON.stringify(event));
        event.accepted = true;
        app.clickSound();
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
        anchors.leftMargin: imageSpacer
        anchors.rightMargin: imageSpacer
        anchors.topMargin: imageSpacer
        anchors.bottomMargin: imageSpacer
        clip: true
        cellWidth: imageSize + imageSpacer
        cellHeight: imageSize + imageSpacer

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
            width: showcasePanelGrid.cellWidth - imageSpacer
            height: showcasePanelGrid.cellHeight - imageSpacer
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
            width: showcasePanelGrid.cellWidth - imageSpacer
            height: showcasePanelGrid.cellHeight - imageSpacer
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



