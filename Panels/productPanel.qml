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
    property int imageSize: height - screenManager.spacer() * 4 - screenManager.buttonSize()
    property int printButtonSize: screenManager.buttonSize() * 2 + screenManager.spacer()
    property bool isPiece: false
    property string productName: ""
    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    Connections // Slot for signal AppManager::showPrinterMessage:
    {
        target: app
        function onShowPrinterMessage(value)
        {
            app.debugLog("@@@@@ productPanel onShowPrinterMessage %1".arg(value))
            productPanelPrintMessage.text = value
        }
    }

    Connections // Slot for signal AppManager::showProductImage:
    {
        target: app
        function onShowProductImage(value)
        {
            app.debugLog("@@@@@ productPanel onShowProductImage %1".arg(value))
            productPanelImage.source = value
        }
    }

    Connections // Slot for signal AppManager::enableManualPrint:
    {
        target: app
        function onEnableManualPrint(value)
        {
            app.debugLog("@@@@@ productPanel onEnableManualPrint %1".arg(value))
            productPanelPrintButton.visible = value
            if(value) productPanelPrintMessageRectangle.color = "transparent"
            else productPanelPrintMessageRectangle.color = Constants.colorError
        }
    }

    Connections // Slot for signal AppManager::resetCurrentProduct:
    {
        target: app
        function onResetCurrentProduct()
        {
            app.debugLog("@@@@@ productPanel onResetCurrentProduct ")
            productPanel.close()
        }
    }

    GridLayout
    {
        id: productPanelLayout
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columnSpacing: screenManager.spacer()
        rowSpacing: screenManager.spacer()
        columns: 4
        rows: 3

        focus: true
        Keys.onPressed: (event) =>
        {
            app.debugLog("@@@@@ productPanel Keys.onPressed %1".arg(JSON.stringify(event)))
            event.accepted = true;
            app.clickSound();
            app.onUserAction();
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                    app.onProductPanelCloseClicked()
                    break
                case Qt.Key_Q: // Поиск
                    app.onMainPageSwiped(2)
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
                    else app.beepSound()
                    break;
                case Qt.Key_Down:
                    if (!productPanelList.atYEnd) productPanelList.flick(0, -Constants.flickVelocity)
                    else app.beepSound()
                    break;
                default:
                    app.beepSound();
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
            fillMode: Image.PreserveAspectFit
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
                font { pointSize: screenManager.largeFontSize(); bold: true }
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
                onFlickStarted: app.onUserAction()

                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: productPanelModel
                delegate: Text
                {
                    leftPadding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize(); family: "Roboto" }
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
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            icon.source: "../Icons/info_outline"
            onClicked: app.onProductDescriptionClicked()
        }

        RoundIconButton
        {
            id: productPanelCloseButton
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/close"
            onClicked: app.onProductPanelCloseClicked()
        }

        RoundTextButton
        {
            id: productPanelPiecesButton
            Layout.preferredWidth: screenManager.buttonSize() * 2 + screenManager.spacer()
            Layout.preferredHeight: screenManager.buttonSize()
            Layout.column: 2
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignTop
            Material.background: Material.color(Material.BlueGrey, isPiece ? Material.Shade200 : Material.Shade700)
            text: isPiece ? qsTr("+ / -") : qsTr(" ")
            font { pointSize: screenManager.largeFontSize() }
            onClicked: app.onProductPanelPiecesClicked()
        }

        RoundIconButton
        {
            id: productPanelPrintButton
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignBottom
            Layout.preferredWidth:  printButtonSize
            Layout.preferredHeight: printButtonSize
            icon { width: screenManager.buttonSize(); height: screenManager.buttonSize(); source: "../Icons/print" }
            Material.background: Constants.colorAuto
            onClicked: app.onPrintClicked()
        }

        Rectangle
        {
            id: productPanelPrintMessageRectangle
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignBottom
            Layout.preferredWidth:  printButtonSize
            Layout.preferredHeight: printButtonSize
            radius: screenManager.spacer()
            color: Constants.colorError

            CardText
            {
                id: productPanelPrintMessage
                verticalAlignment: Text.AlignBottom
                bottomPadding: screenManager.spacer() / 2
                color: Constants.colorWhite
            }
        }

    }
}

