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
    property int buttonPanelWidth: screenManager.buttonSize() + screenManager.spacer() * 2
    property int imageSize: (width - buttonPanelWidth) / screenManager.showcaseRowImages()
    focus: true

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === 0) showcasePanel.forceActiveFocus()
        }
    }

    Connections // Slot for signal AppManager::showShowcaseSort
    {
        target: app
        function onShowShowcaseSort(sort, increase)
        {
            showcasePanelCodeButton.  marked = sort === 0;
            showcasePanelNumberButton.marked = sort === 1;
            showcasePanelNameButton.  marked = sort === 2;
            if(increase) showcasePanelDirectionButton.icon.source = "../Icons/arrow_up"
            else         showcasePanelDirectionButton.icon.source = "../Icons/arrow_down"
            showcasePanelDirectionButton.marked = !increase
        }
    }

    Keys.onPressed: (event) =>
    {
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
            case Qt.Key_Right: case Qt.Key_Q:
                app.onMainPageSwiped(1)
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
                icon.source: "../Icons/code"
                onClicked: app.onShowcaseSortClicked(0)
            }

            Spacer {}

            RoundIconButton
            {
                id: showcasePanelNumberButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/number"
                onClicked: app.onShowcaseSortClicked(1)
            }

            Spacer {}

            RoundIconButton
            {
                id: showcasePanelNameButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/a"
                onClicked: app.onShowcaseSortClicked(2)
            }

            Spacer {}

            RoundIconButton
            {
                id: showcasePanelDirectionButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/arrow_up"
                onClicked: app.onShowcaseDirectionClicked()
            }
        }

        GridView
        {
            id: showcasePanelGrid
            width: parent.width - buttonPanelWidth
            height: parent.height
            leftMargin: 0
            topMargin: 0
            rightMargin: 0
            bottomMargin: 0
            clip: true
            cellWidth: imageSize
            cellHeight: imageSize
            /*
            ScrollBar.vertical: ScrollBar
            {
                width: screenManager.scrollBarWidth()
                background: Rectangle { color: "transparent" }
                policy: ScrollBar.AlwaysOn
            }
            */
            model: showcasePanelModel
            delegate: GridLayout
            {
                width: imageSize
                height: imageSize
                columnSpacing: 0
                rowSpacing: 0

                Image
                {
                    Layout.column: 0
                    Layout.row: 0
                    Layout.rowSpan: 5
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredWidth: imageSize - 2
                    Layout.preferredHeight: imageSize - 2
                    fillMode: Image.PreserveAspectFit
                    source: model.image

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: app.onShowcaseClicked(index)
                    }
                }

                Rectangle
                {
                    Layout.column: 0
                    Layout.row: 0
                    Layout.rowSpan: 5
                    Layout.fillWidth: parent
                    Layout.fillHeight: parent
                    color: "transparent"
                    border.width: 1
                    border.color: Material.color(Material.BlueGrey, Material.Shade100)
                }

                Spacer
                {
                    Layout.column: 0
                    Layout.row: 0
                    height: 1
                }

                Sticker
                {
                    Layout.column: 0
                    Layout.row: 1
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: model.top
                    visible: model.top !== ""
                }

                Sticker
                {
                    Layout.column: 0
                    Layout.row: 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: model.bottom
                    visible: model.bottom !== ""
                }

                Spacer
                {
                    Layout.column: 0
                    Layout.row: 4
                    height: 1
                }
            }
        }
    }
}



