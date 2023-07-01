import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

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
    property int imageSize: height - Constants.margin * 2
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

    Connections // Slot for signal AppManager::enablePrint:
    {
        target: app
        function onEnablePrint(value)
        {
            console.debug("@@@@@ productPanel onEnablePrint ", value)
            productPanelPrintButton.visible = value
            productPanelPrintMessageRectangle.visible = !value
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
            switch (event.key)
            {
                case Qt.Key_Escape: // Круглая стрелка
                    app.onProductPanelCloseClicked()
                    break
                case Qt.Key_Q:
                    app.onShowMainPage(2)
                    app.onProductPanelCloseClicked()
                    break;
                case Qt.Key_T: // >T<
                    app.onWeightParamClicked(1)
                    break
                case Qt.Key_Z: // >0<
                    app.onWeightParamClicked(0)
                    break
                case Qt.Key_F8: // Печать
                    app.onPrint()
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
            Layout.row: 0
            Layout.rowSpan: 3
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            source: "../Images/image_dummy"
        }

        Rectangle
        {
            Layout.fillWidth: parent
            Layout.column: 1
            Layout.row: 0
            Layout.columnSpan: 1
            color: "transparent"

            CardTitleText
            {
                color: Constants.colorWhite
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
            onClicked: app.onProductDescriptionClicked() // AppManager's slot
        }

        RoundIconButton
        {
            id: productPanelCloseButton
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            icon.source: "../Icons/close_black_48"
            onClicked: app.onProductPanelCloseClicked() // AppManager's slot
        }

        RoundTextButton
        {
            id: productPanelPiecesButton
            Layout.preferredWidth: Constants.buttonSize * 2 + Constants.margin
            Layout.preferredHeight: Constants.buttonSize
            Layout.column: 2
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Material.background: Material.color(Material.BlueGrey, isPiece ? Material.Shade200 : Material.Shade700)
            text: isPiece ? qsTr("+ / -") : qsTr(" ")
            font { pointSize: Constants.hugeFontSize }
            onClicked: app.onProductPanelPiecesClicked() // AppManager's slot
        }

        RoundIconButton
        {
            id: productPanelPrintButton
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            width:  Constants.buttonSize * 2 + productPanelLayout.rowSpacing
            height: Constants.buttonSize * 2 + productPanelLayout.rowSpacing
            icon { width: Constants.maxIconSize; height: Constants.maxIconSize; source: "../Icons/print_black_48" }
            Material.background: Constants.colorAuto
            onClicked: app.onPrint() // AppManager's slot
        }

        Rectangle
        {
            id: productPanelPrintMessageRectangle
            Layout.column: 2
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            width: productPanelPrintButton.width
            height: productPanelPrintButton.height
            radius: Constants.margin
            color: Constants.colorError

            CardText
            {
                id: productPanelPrintMessage
                color: Constants.colorWhite
            }
        }
    }
}

