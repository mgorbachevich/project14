import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id: showcasePanel
    Material.background: Material.color(Material.Grey, Material.Shade100)
    color: Material.background
    property int buttonPanelWidth: screenManager.buttonSize() + screenManager.spacer() * 2
    property int imageSize: (width - buttonPanelWidth) / screenManager.showcaseRowImages()
    property int imageBorderWidth: 1
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

    Connections // Slot for signal AppManager::showControlParam:
    {
        target: app
        function onShowControlParam(param, value)
        {
            switch (param)
            {
            case 16: // IsTareValue
                showcasePanelAutoButton.visible = value !== '0'
                showcasePanelAutoButton.marked = value === '1'
                break
            case 32: // IsSelfService
                showcasePanelSelfService.visible = value !== '0'
                showcasePanelLayout.visible = value === '0'
                break
            case 33: showcasePanelTitle.text = value; break;
            case 34: showcasePanelDeleteButton.visible = value !== '0'; break;
            }
        }
    }

    Keys.onPressed: (event) =>
    {
        event.accepted = true;
        app.onClick();
        switch (event.key)
        {
            case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
            case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                app.onClicked3(29, event.text);
                break;
            case Qt.Key_F9: // Ключ
                app.onClicked(7)
                break;
            case Qt.Key_T: // >T<
                app.onClicked(10)
                break
            case Qt.Key_Z: // >0<
                app.onClicked(9)
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
                app.onClicked(26)
                break
            default:
                app.beepSound();
                break;
        }
    }

    Label
    {
        id: showcasePanelSelfService
        width: parent.width
        height: parent.height
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font { pointSize: screenManager.extraLargeFontSize() }
        wrapMode: Text.WordWrap
        text: "Выберите товар.\n\nПоложите товар на весы."
    }

    Row
    {
        id: showcasePanelLayout
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
                onClicked: app.onClicked2(21, 0)
            }

            Spacer { height: screenManager.spacer() / 4 }

            RoundIconButton
            {
                id: showcasePanelNumberButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/number"
                onClicked: app.onClicked2(21, 1)
            }

            Spacer { height: screenManager.spacer() / 4 }

            RoundIconButton
            {
                id: showcasePanelNameButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/name"
                onClicked: app.onClicked2(21, 2)
            }

            Spacer { height: screenManager.spacer() / 4 }

            RoundIconButton
            {
                id: showcasePanelDirectionButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/arrow_up"
                onClicked: app.onClicked(12)
            }

            Spacer { height: screenManager.spacer() / 4 }

            RoundIconButton
            {
                id: showcasePanelAutoButton
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: "../Icons/auto"
                onClicked: app.onClicked(14)
            }
        }

        Column
        {
            width: parent.width - buttonPanelWidth
            height: parent.height
            spacing: 0

            Rectangle
            {
                id: showcasePanelTitleRectangle
                width: parent.width
                height: screenManager.buttonSize() + screenManager.spacer() * 2
                color: Material.color(Material.Grey, Material.Shade100)

                Row
                {

                    width: parent.width
                    height: parent.height

                    CardTitleText
                    {
                        id: showcasePanelTitle
                        text: qsTr("")
                    }

                    RoundIconButton
                    {
                        id: showcasePanelDeleteButton
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        icon.source: "../Icons/delete"
                        onClicked: app.onClicked(31)
                    }
                }
            }

            GridView
            {
                id: showcasePanelGrid
                width: parent.width
                height: parent.height - showcasePanelTitleRectangle.height
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
                        Layout.preferredWidth: imageSize - imageBorderWidth * 2
                        Layout.preferredHeight: imageSize - imageBorderWidth * 2
                        fillMode: Image.PreserveAspectFit
                        source: model.image

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: app.onClicked2(22, index)
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
                        border.width: imageBorderWidth
                        border.color: Material.color(Material.Grey, Material.Shade100)
                    }

                    Spacer
                    {
                        Layout.column: 0
                        Layout.row: 0
                        height: imageBorderWidth + 1
                    }

                    Sticker
                    {
                        Layout.column: 0
                        Layout.row: 1
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.preferredWidth: imageSize - imageBorderWidth * 2 - 2
                        text: model.top
                        visible: model.top !== ""
                    }

                    Sticker
                    {
                        Layout.column: 0
                        Layout.row: 2
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.preferredWidth: imageSize - imageBorderWidth * 2 - 2
                        wrapMode: Text.WordWrap
                        maximumLineCount: 3
                        width: imageSize
                        height: imageSize
                        text: model.center
                        visible: model.center !== ""
                    }

                    Sticker
                    {
                        Layout.column: 0
                        Layout.row: 3
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.preferredWidth: imageSize - imageBorderWidth * 2 - 2
                        width: imageSize
                        text: model.bottom
                        visible: model.bottom !== ""
                    }

                    Spacer
                    {
                        Layout.column: 0
                        Layout.row: 4
                        height: imageBorderWidth + 1
                    }
                }
            }
        }
    }
}



