import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: productPanel
    padding : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    modal: true
    dim: false
    Material.theme: Material.Dark
    Material.background: Material.color(Material.Grey, Material.Shade800)
    property int imageSize: height - Constants.margin * 4 - Constants.buttonSize
    property int printButtonSize: Constants.buttonSize * 2 + Constants.margin
    property bool isPiece: false
    property string productName: ""
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::showPrinterMessage:
    {
        target: app
        function onShowPrinterMessage(value)
        {
            console.debug("@@@@@ productPanel onShowPrinterMessage ", value)
            productPanelPrintMessage.text = value
        }
    }

    Connections // Slot for signal AppManager::showProductImage:
    {
        target: app
        function onShowProductImage(value)
        {
            console.debug("@@@@@ productPanel onShowProductImage ", value)
            productPanelImage.source = value
        }
    }

    Connections // Slot for signal AppManager::enableManualPrint:
    {
        target: app
        function onEnableManualPrint(value)
        {
            console.debug("@@@@@ productPanel onEnableManualPrint ", value)
            productPanelPrintButton.visible = value
        }
    }

    Connections // Slot for signal AppManager::resetCurrentProduct:
    {
        target: app
        function onResetCurrentProduct()
        {
            console.debug("@@@@@ productPanel onResetCurrentProduct ")
            productPanel.close()
        }
    }

    GridLayout
    {
        id: productPanelLayout
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 4
        rows: 3

        focus: true
        Keys.onPressed: (event) =>
        {
            console.debug("@@@@@ productPanel Keys.onPressed ", JSON.stringify(event))
            event.accepted = true;
            app.onUserAction(); // AppManager's slot
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                    app.onProductPanelCloseClicked()
                    break
                case Qt.Key_Q: // Поиск
                    app.onSwipeMainPage(2)
                    app.onProductPanelCloseClicked()
                    break;
                case Qt.Key_T: // >T<
                    app.onTareClicked()
                    break
                case Qt.Key_Z: // >0<
                    app.onZeroClicked()
                    break
                case Qt.Key_F8: // Печать
                    app.onPrintClicked()
                    break
                case Qt.Key_F10: // Промотка
                    app.onRewind()
                    break
                case Qt.Key_Up:
                    if (!productPanelList.atYBeginning) productPanelList.flick(0, Constants.flickVelocity)
                    else app.onBeep()
                    break;
                case Qt.Key_Down:
                    if (!productPanelList.atYEnd) productPanelList.flick(0, -Constants.flickVelocity)
                    else app.onBeep()
                    break;
                default:
                    app.onBeep();
                    break
            }
        }

        Image
        {
            id: productPanelImage
            Layout.preferredWidth: productPanel.imageSize
            Layout.preferredHeight: productPanel.imageSize
            Layout.column: 0
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignCenter
            source: "../Images/image_dummy"
        }

        Rectangle
        {
            Layout.fillWidth: parent
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            color: "transparent"

            CardTitleText
            {
                color: Constants.colorWhite
                font { pointSize: Constants.hugeFontSize; bold: true }
                text: productName
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: "transparent"

            ListView
            {
                id: productPanelList
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

                model: productPanelModel
                delegate: Text
                {
                    leftPadding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    color: Material.color(Material.BlueGrey, Material.Shade50)
                    text: model.value // Roles::ValueRole
                }
            }
        }

        RoundIconButton
        {
            id: productPanelInfoButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            icon.source: "../Icons/info_outline_black_48"
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onProductDescriptionClicked() // AppManager's slot
            }
        }

        RoundIconButton
        {
            id: productPanelCloseButton
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            icon.source: "../Icons/close_black_48"
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onProductPanelCloseClicked() // AppManager's slot
            }
        }

        RoundTextButton
        {
            id: productPanelPiecesButton
            Layout.preferredWidth: Constants.buttonSize * 2 + Constants.margin
            Layout.preferredHeight: Constants.buttonSize
            Layout.column: 2
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            Material.background: Material.color(Material.BlueGrey, isPiece ? Material.Shade200 : Material.Shade700)
            text: isPiece ? qsTr("+ / -") : qsTr(" ")
            font { pointSize: Constants.hugeFontSize }
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onProductPanelPiecesClicked() // AppManager's slot
            }
        }

        Rectangle
        {
            id: productPanelPrintMessageRectangle
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth:  printButtonSize
            Layout.preferredHeight: printButtonSize
            radius: Constants.margin
            color: Constants.colorError

            CardText
            {
                id: productPanelPrintMessage
                color: Constants.colorWhite
            }
        }

        RoundIconButton
        {
            id: productPanelPrintButton
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth:  printButtonSize
            Layout.preferredHeight: printButtonSize
            icon { width: Constants.maxIconSize; height: Constants.maxIconSize; source: "../Icons/print_black_48" }
            Material.background: Constants.colorAuto
            onClicked:
            {
                app.onUserAction(); // AppManager's slot
                app.onPrintClicked() // AppManager's slot
            }
        }
    }
}

