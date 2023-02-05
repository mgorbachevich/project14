import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    // https://doc.qt.io/qt-5/qtquickcontrols2-material.html
    Material.theme: Material.Dark
    Material.background: Material.color(Material.BlueGrey, Material.Shade900)
    Material.foreground: Material.color(Material.Grey, Material.Shade50)
    Material.accent: Material.Orange
    Material.primary: Material.color(Material.Indigo, Material.Shade100)

    id: productPanel
    padding : 0
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    focus: true
    modal: true
    dim: false
    property int imageSize: (height - Constants.margin * 3) / 2

    Connections // Slot for signal AppManager::showProductImage:
    {
        target: app
        function onShowProductImage(value) { productPanelImage.source = value }
    }

    GridLayout
    {
        id: productPanelLayout
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 4
        rows: 2

        Image
        {
            id: productPanelImage
            Layout.preferredWidth: productPanel.imageSize
            Layout.preferredHeight: productPanel.imageSize
            Layout.column: 0
            Layout.row: 0
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            source: "../Images/image_fruit1"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
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
                    color: Material.color(Material.Grey, Material.Shade50)
                    text: model.value // Roles::ValueRole
                }
            }
        }

        Button
        {
            id: productPanelInfoButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/info_outline_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
            onClicked: app.onProductDescriptionClicked() // AppManager's slot
        }

        Button
        {
            id: productPanelCloseButton
            Layout.column: 3
            Layout.row: 0
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/close_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
            onClicked:
            {
                app.onProductPanelClosed() // AppManager's slot
                productPanel.close()
            }
        }

        Button
        {
            id: productPanelPrintButton
            Layout.column: 2
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignRight | Qt.AlignBottom
            Layout.preferredWidth:  Constants.buttonSize * 2 + productPanelLayout.rowSpacing
            Layout.preferredHeight: Constants.buttonSize * 2 + productPanelLayout.rowSpacing
            icon { width: Constants.maxIconSize; height: Constants.maxIconSize; source: "../Icons/print_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.accent
            onClicked: app.onPrint() // AppManager's slot
        }
    }
}
