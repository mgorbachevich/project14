import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Rectangle
{
    // https://doc.qt.io/qt-5/qtquickcontrols2-material.html
    Material.theme: Material.Dark
    Material.background: Material.color(Material.BlueGrey, Material.Shade900)
    Material.foreground: Material.color(Material.Grey, Material.Shade50)
    Material.accent: Material.Orange
    Material.primary: Material.color(Material.Indigo, Material.Shade100)

    id:  weightPanel
    objectName: "weightPanel"
    height: valueFontSize + titleFontSize + Constants.margin
    color: Material.background

    property int valueFontSize: 70
    property int titleFontSize: 11
    property int flagSize: (valueFontSize + titleFontSize) / 3
    property int flagWidth: flagSize + Constants.margin
    property int displayWidth: weightPanel.width - flagWidth - Constants.margin
    property int weightWidth: displayWidth * 6 / (6 + 7 + 9)
    property int priceWidth: displayWidth * 7 / (6 + 7 + 9)
    property int amountWidth: displayWidth - weightWidth - priceWidth

    Connections // Slot for signal AppManager::showWeightValue:
    {
        target: app
        function onShowWeightValue(param, value)
        {
            switch (param)
            {
            case 0:
                weightPanelWeightValue.text = value
                break
            case 1:
                weightPanelPriceValue.text = value
                break;
            case 2:
                weightPanelAmountValue.text = value
                break;
            case 3:
                weightPanelWeightValue.color = value
                break
            case 4:
                weightPanelPriceValue.color = value
                break
            case 5:
                weightPanelAmountValue.color = value
                break
            case 6:
                weightPanelWeightTitle.text = value
                break
            case 7:
                weightPanelPriceTitle.text = value
                break
            case 8:
                weightPanelAmountTitle.text = value
                break
            }
        }
    }

    Connections // Slot for signal AppManager::showWeightParam:
    {
        target: app
        function onShowWeightParam(param, value)
        {
            switch (param)
            {
            case 0: // >0<
                if (value) productPanelZeroIcon.source = "../Icons/0_48"
                else productPanelZeroIcon.source = "../Icons/empty_48"
                break
            case 1: // >T<
                if (value) productPanelTareIcon.source = "../Icons/tare_48"
                else productPanelTareIcon.source = "../Icons/empty_48"
                break
            case 2: // Auto
                if (value) productPanelAutoWeightIcon.source = "../Icons/weight_48"
                else productPanelAutoWeightIcon.source = "../Icons/empty_48"
                break
            }
        }
    }

    Row
    {
        anchors.fill: parent
        spacing: 0
        padding: 0

        Rectangle
        {
            width: flagWidth
            height: parent.height
            color: Material.background

            Column
            {
                anchors.fill: parent
                topPadding: Constants.margin / 2
                spacing: 0
                padding: 0

                Image
                {
                    id: productPanelAutoWeightIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/empty_48"
                }

                Image
                {
                    id: productPanelZeroIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/empty_48"
                }

                Image
                {
                    id: productPanelTareIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/empty_48"
                }
            }
        }

        Column
        {
            width: weightWidth
            height: parent.height
            spacing: 0
            padding: 0

            Text
            {
                id: weightPanelWeightTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelWeightValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
            }
        }

        Column
        {
            width: priceWidth
            height: parent.height
            spacing: 0
            padding: 0

            Text
            {
                id: weightPanelPriceTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelPriceValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
            }
        }

        Column
        {
            width: amountWidth
            height: parent.height
            spacing: 0
            padding: 0

            Text
            {
                id: weightPanelAmountTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelAmountValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
            }
        }
    }
}


