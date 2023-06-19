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
    property int flagIconSize: (valueFontSize + titleFontSize) / 4
    property int flagIconWidth: flagIconSize + Constants.margin
    property int displayWidth: weightPanel.width - flagIconWidth - Constants.margin
    property int weightWidth: displayWidth * 6 / (6 + 7 + 9)
    property int priceWidth: displayWidth * 7 / (6 + 7 + 9)
    property int amountWidth: displayWidth - weightWidth - priceWidth

    Connections // Slot for signal AppManager::showWeightParam:
    {
        target: app
        function onShowWeightParam(param, value)
        {
            switch (param)
            {
            case 0: // WeightParam_ZeroFlag >0<
                if (value === 'true') productPanelZeroIcon.source = "../Icons/0_48"
                else productPanelZeroIcon.source = "../Icons/empty_48"
                break
            case 1: // WeightParam_TareFlag >T<
                if (value === 'true') productPanelTareIcon.source = "../Icons/tare_48"
                else productPanelTareIcon.source = "../Icons/empty_48"
                break
            case 2: // WeightParam_AutoFlag
                if (value === 'true') productPanelAutoWeightIcon.source = "../Icons/weight_48"
                else productPanelAutoWeightIcon.source = "../Icons/empty_48"
                break
            case 13: // WeightParam_ErrorFlag
                if (value === 'true') productPanelErrorIcon.source = "../Icons/error_48"
                else productPanelAutoWeightIcon.source = "../Icons/empty_48"
                break
            case 3: // WeightParam_TareValue
                break
            case 4: // WeightParam_WeightValue
                weightPanelWeightValue.text = value
                break
            case 5: // WeightParam_PriceValue
                weightPanelPriceValue.text = value
                break;
            case 6: // WeightParam_AmountValue
                weightPanelAmountValue.text = value
                break;
            case 7: // WeightParam_WeightColor
                weightPanelWeightValue.color = value
                break
            case 8: // WeightParam_PriceColor
                weightPanelPriceValue.color = value
                break
            case 9: // WeightParam_AmountColor
                weightPanelAmountValue.color = value
                break
            case 10: // WeightParam_WeightTitle
                weightPanelWeightTitle.text = value
                break
            case 11: // WeightParam_PriceTitle
                weightPanelPriceTitle.text = value
                break
            case 12: // WeightParam_AmountTitle
                weightPanelAmountTitle.text = value
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
            width: flagIconWidth
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
                    id: productPanelErrorIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_48"
                }

                Image
                {
                    id: productPanelAutoWeightIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_48"
                }

                Image
                {
                    id: productPanelZeroIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_48"
                }

                Image
                {
                    id: productPanelTareIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
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


