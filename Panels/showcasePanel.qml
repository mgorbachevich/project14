import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id:  showcasePanel
    Material.background: Material.color(Material.Grey, Material.Shade100)
    color: Material.background
    property int spacer: 1
    property int buttonPanelWidth: screenManager.buttonSize() + screenManager.spacer() * 2
    //property int imageSize: (width - screenManager.scrollBarWidth() - buttonPanelWidth - spacer * 2) / screenManager.showcaseRowImages() - spacer
    property int imageSize: (width - buttonPanelWidth - screenManager.showcaseRowImages() * spacer) / screenManager.showcaseRowImages()
    focus: true

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === 0)
            {
                app.debugLog("@@@@@ showcasePanel onShowMainPage");
                showcasePanel.forceActiveFocus()
            }
        }
    }

    Connections // Slot for signal AppManager::showShowcaseSort
    {
        target: app
        function onShowShowcaseSort(value)
        {
            app.debugLog("@@@@@ showcasePanel onSetShowcaseSort");
            switch (value)
            {
                case 0:
                    showcasePanelCodeButton.Material.foreground = Material.Orange
                    showcasePanelNameButton.Material.foreground = Material.color(Material.BlueGrey, Material.Shade900)
                    break;
                case 1:
                    showcasePanelCodeButton.Material.foreground = Material.color(Material.BlueGrey, Material.Shade900)
                    showcasePanelNameButton.Material.foreground = Material.Orange
                    break;
            }
        }
    }

    Keys.onPressed: (event) =>
    {
        app.debugLog("@@@@@ showcasePanel Keys.onPressed %1".arg(JSON.stringify(event)))
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
                app.onMainPageSwiped(1)
                break;
            case Qt.Key_Q:
                app.onMainPageSwiped(2)
                break;
            case Qt.Key_Up:
                if (!showcasePanelGrid.atYBeginning) showcasePanelGrid.flick(0, screenManager.flickVelocity())
                else app.beepSound()
                break;
            case Qt.Key_Down:
                if (!showcasePanelGrid.atYEnd) showcasePanelGrid.flick(0, -screenManager.flickVelocity())
                else app.beepSound()
                break;
            case Qt.Key_F10: // Промотка
                app.onRewind()
                break
            default:
                app.beepSound();
                break;
        }
    }

    Row
    {
        width: parent.width
        height: parent.height

        Column
        {
            width: buttonPanelWidth
            height: parent.height
            spacing: 0

            Spacer {}

            RoundIconButton
            {
                id: showcasePanelCodeButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/n"
                onClicked: app.onShowcaseSortClicked(0)
            }

            Spacer {}

            RoundIconButton
            {
                id: showcasePanelNameButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/a"
                onClicked: app.onShowcaseSortClicked(1)
            }
        }

        GridView
        {
            id: showcasePanelGrid
            width: parent.width - buttonPanelWidth
            height: parent.height
            leftMargin: 0
            topMargin: spacer
            rightMargin: spacer
            bottomMargin: spacer
            clip: true
            cellWidth: imageSize + spacer
            cellHeight: imageSize + spacer
            /*
            ScrollBar.vertical: ScrollBar
            {
                width: screenManager.scrollBarWidth()
                background: Rectangle { color: "transparent" }
                policy: ScrollBar.AlwaysOn
            }
            */
            model: showcasePanelModel
            delegate: Label
            {
                width: imageSize
                height: imageSize
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
}



