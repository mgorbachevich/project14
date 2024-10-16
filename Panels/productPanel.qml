import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes

Popup
{
    id: productPanel
    padding : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    modal: false
    dim: false
    Material.background: Material.color(Material.Grey, Material.Shade900)
    property int imageSize: height - screenManager.buttonSize() - screenManager.spacer()
    property int printButtonSize: screenManager.buttonSize() * 2 + screenManager.spacer()
    property bool isPiece: false
    property string productName: ""

    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    enter: Transition { enabled: false }
    exit: Transition { enabled: false }

    Connections // Slot for signal AppManager::showControlParam:
    {
        target: app
        function onShowControlParam(param, value)
        {
            if(param === 17) productPanelPrintMessage.text = value
            if(param === 19) productPanelImage.source = value
        }
    }

    Connections // Slot for signal AppManager::enableManualPrint:
    {
        target: app
        function onEnableManualPrint(value)
        {
            //app.debugLog("@@@@@ productPanel onEnableManualPrint %1".arg(value))
            productPanelPrintButton.visible = value
            if(value) productPanelPrintMessageRectangle.color = "transparent"
            else productPanelPrintMessageRectangle.color = Material.color(Material.Red)
        }
    }

    Connections // Slot for signal AppManager::resetCurrentProduct:
    {
        target: app
        function onResetCurrentProduct()
        {
            //app.debugLog("@@@@@ productPanel onResetCurrentProduct ")
            productPanel.close()
        }
    }

    Connections // Slot for signal AppManager::setCurrentProductFavorite:
    {
        target: app
        function onSetCurrentProductFavorite(value)
        {
            //app.debugLog("@@@@@ productPanel onSetCurrentProductFavorite ")
            productPanelFavoriteButton.marked = value;
        }
    }

    GridLayout
    {
        id: productPanelLayout
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        focus: true
        Keys.onPressed: (event) =>
        {
            //app.debugLog("@@@@@ productPanel Keys.onPressed %1".arg(JSON.stringify(event)))
            event.accepted = true;
            app.onClick();
            switch (event.key)
            {
                case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                    break;
                case Qt.Key_Escape: // Круглая стрелка
                    app.onClicked(18)
                    break
                case Qt.Key_Q: // Поиск
                    app.onMainPageSwiped(1)
                    app.onClicked(18)
                    break;
                case Qt.Key_T: // >T<
                    app.onClicked(10)
                    break
                case Qt.Key_Z: // >0<
                    app.onClicked(9)
                    break
                case Qt.Key_F8: // Печать
                    app.onClicked(15)
                    break
                case Qt.Key_F10: // Промотка
                    app.onClicked(26)
                    break
                case Qt.Key_Up:
                    if (!productPanelList.atYBeginning) productPanelList.flick(0, screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!productPanelList.atYEnd) productPanelList.flick(0, -screenManager.flickVelocity())
                    else app.beepSound()
                    break;
                default:
                    app.beepSound();
                    break
            }
        }

        Rectangle
        {
            Layout.fillWidth: parent
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            color: "red"

            CardTitleText
            {
                color: "white"
                font { pointSize: screenManager.largeFontSize(); bold: true }
                text: productName
            }
        }

        RoundIconButton
        {
            id: productPanelFavoriteButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            icon.source: "../Icons/star"
            onClicked: app.onClicked(17)
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 2
            Layout.rowSpan: 4
            Layout.preferredHeight: imageSize
            Layout.preferredWidth: imageSize
            color: Material.color(Material.Grey, Material.Shade800)

            Image
            {
                id: productPanelImage
                width: imageSize
                height: imageSize
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "../Images/dummy"
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 2
            Layout.rowSpan: 4
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade800)

            ListView
            {
                id: productPanelList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */
                model: productPanelModel
                delegate: Text
                {
                    leftPadding: screenManager.spacer() * 2
                    rightPadding: screenManager.spacer()
                    font.pointSize: screenManager.normalFontSize()
                    color: Material.color(Material.BlueGrey, Material.Shade50)
                    text: model.value // Roles::ValueRole
                }
            }
        }

        Spacer
        {
            Layout.column: 3
            Layout.row: 0
        }

        RoundIconButton
        {
            id: productPanelInfoButton
            Layout.column: 4
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            icon.source: "../Icons/info_outline"
            onClicked: app.onClicked(16)
        }

        Spacer
        {
            Layout.column: 5
            Layout.row: 0
        }

        RoundIconButton
        {
            id: productPanelCloseButton
            Layout.column: 6
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/close"
            onClicked: app.onClicked(18)
        }

        Spacer
        {
            Layout.column: 7
            Layout.row: 0
        }

        Spacer
        {
            Layout.column: 3
            Layout.row: 1
        }

        RoundTextButton
        {
            id: productPanelPiecesButton
            Layout.preferredWidth: screenManager.buttonSize() * 2 + screenManager.spacer()
            Layout.preferredHeight: screenManager.buttonSize()
            Layout.column: 4
            Layout.row: 2
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignTop
            Material.background: Material.color(Material.BlueGrey, isPiece ? Material.Shade100 : Material.Shade700)
            text: isPiece ? qsTr("+ / -") : qsTr(" ")
            font { pointSize: screenManager.largeFontSize() }
            onClicked: app.onClicked(19)
        }

        Spacer
        {
            Layout.column: 3
            Layout.row: 3
        }

        RoundIconButton
        {
            id: productPanelPrintButton
            Layout.column: 4
            Layout.row: 4
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignBottom
            Layout.preferredWidth: printButtonSize
            Layout.preferredHeight: printButtonSize
            icon { width: screenManager.buttonSize(); height: screenManager.buttonSize(); source: "../Icons/print" }
            Material.background: Material.color(Material.BlueGrey, Material.Shade200)
            onClicked: app.onClicked(15)
        }

        Rectangle
        {
            id: productPanelPrintMessageRectangle
            Layout.column: 4
            Layout.row: 4
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignBottom
            Layout.preferredWidth:  printButtonSize
            Layout.preferredHeight: printButtonSize
            radius: screenManager.spacer()
            color: Material.color(Material.Red)

            CardText
            {
                id: productPanelPrintMessage
                verticalAlignment: Text.AlignBottom
                bottomPadding: screenManager.spacer() / 2
                color: "white"
            }
        }

        Spacer
        {
            Layout.column: 3
            Layout.row: 5
        }
    }
}

