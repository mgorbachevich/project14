import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    id:  weightPanel
    height: valueFontSize + titleFontSize + Constants.margin
    Material.background: Material.color(Material.Grey, Material.Shade900)
    color: Material.background

    property int valueFontSize: 70
    property int titleFontSize: 10
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
            case 1: // TareFlag >T<
                if (value === 'true') productPanelTareIcon.source = "../Icons/tare_flag_48"
                else productPanelTareIcon.source = "../Icons/empty_flag_48"
                break
            case 2: // ZeroFlag >0<
                if (value === 'true') productPanelZeroIcon.source = "../Icons/zero_flag_48"
                else productPanelZeroIcon.source = "../Icons/empty_flag_48"
                break
            case 3: // TareValue todo
                break
            case 4: // WeightValue
                weightPanelWeightValue.text = value
                break
            case 5: // PriceValue
                weightPanelPriceValue.text = value
                break;
            case 6: // AmountValue
                weightPanelAmountValue.text = value
                break;
            case 7: // WeightColor
                weightPanelWeightValue.color = value
                break
            case 8: // PriceColor
                weightPanelPriceValue.color = value
                break
            case 9: // AmountColor
                weightPanelAmountValue.color = value
                break
            case 10: // WeightTitle
                weightPanelWeightTitle.text = value
                break
            case 11: // PriceTitle
                weightPanelPriceTitle.text = value
                break
            case 12: // AmountTitle
                weightPanelAmountTitle.text = value
                break
            case 13: // WeightError
                if (value === 'true') productPanelErrorIcon.source = "../Icons/error_flag_48"
                else productPanelErrorIcon.source = "../Icons/empty_flag_48"
                break
            case 14: // WeightFixed
            case 15: // PrintError
                break
            case 16: // AutoPrint
                if (value === 'true') productPanelAutoPrintIcon.source = "../Icons/auto_flag_48"
                else productPanelAutoPrintIcon.source = "../Icons/empty_flag_48"
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
            color: "transparent"

            Column
            {
                anchors.fill: parent
                spacing: 0
                padding: 0

                Image
                {
                    id: productPanelErrorIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_flag_48"
                }

                Image
                {
                    id: productPanelAutoPrintIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_flag_48"
                }

                Image
                {
                    id: productPanelTareIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_flag_48"
                }

                Image
                {
                    id: productPanelZeroIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty_flag_48"
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
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelWeightValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                color: Material.foreground
                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        // app.onUserAction(); // AppManager's slot
                        app.onWeightPanelClicked(1); // AppManager's slot
                    }
                }
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
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelPriceValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                color: Material.foreground
                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        // app.onUserAction(); // AppManager's slot
                        app.onWeightPanelClicked(2); // AppManager's slot
                    }
                }
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
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelAmountValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                color: Material.foreground
                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        // app.onUserAction(); // AppManager's slot
                        app.onWeightPanelClicked(3); // AppManager's slot
                    }
                }
            }
        }
    }
}


