import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id:  weightPanel
    property int valueFontSize: screenManager.weightValueFontSize()
    property int titleFontSize: screenManager.weightTitleFontSize()
    property int flagIconSize: (valueFontSize + titleFontSize + screenManager.spacer()) / 4
    property int flagIconWidth: flagIconSize + screenManager.spacer()
    property int displayWidth: weightPanel.width - flagIconWidth - screenManager.spacer()
    property int weightWidth: displayWidth * 6 / (6 + 7 + 9)
    property int priceWidth: displayWidth * 7 / (6 + 7 + 9)
    property int amountWidth: displayWidth - weightWidth - priceWidth

    height: valueFontSize + titleFontSize + screenManager.spacer()
    Material.background: Material.color(Material.Grey, Material.Shade900)
    color: Material.background

    Connections // Slot for signal AppManager::showControlParam:
    {
        target: app
        function onShowControlParam(param, value)
        {
            switch (param)
            {
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
            case 15: // PrintError
                break
            case 1: // TareFlag >T<
                if (value === 'true') weightPanelIcon3.source = "../Icons/tare_white"
                else                  weightPanelIcon3.source = "../Icons/empty"
                break
            case 2: // ZeroFlag >0<
                if (value === 'true') weightPanelIcon4.source = "../Icons/zero_white"
                else                  weightPanelIcon4.source = "../Icons/empty"
                break
            case 14: // WeightFixed
                if (value === 'true') weightPanelIcon2.source = "../Icons/fix_white"
                else                  weightPanelIcon2.source = "../Icons/empty"
                break
            case 13: // WeightError
                if (value === 'true') weightPanelIcon1.source = "../Icons/error_red"
                else                  weightPanelIcon1.source = "../Icons/empty"
                break
            case 16: // AutoPrint
                if (value === 'true') weightPanelIcon1.source = "../Icons/auto_white"
                else                  weightPanelIcon1.source = "../Icons/empty"
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
                    id: weightPanelIcon1
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty"
                }

                Image
                {
                    id: weightPanelIcon2
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty"
                }

                Image
                {
                    id: weightPanelIcon3
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty"
                }

                Image
                {
                    id: weightPanelIcon4
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagIconSize
                    height: flagIconSize
                    source: "../Icons/empty"
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
                    onClicked: app.onWeightPanelClicked(1);
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
                    onClicked: app.onWeightPanelClicked(2);
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
                    onClicked: app.onWeightPanelClicked(3);
                }
            }
        }
    }
}


