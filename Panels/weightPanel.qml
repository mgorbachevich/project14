import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id: weightPanel
    property int valueFontSize: screenManager.weightValueFontSize()
    property int titleFontSize: screenManager.weightTitleFontSize()
    property int flagIconSize: (valueFontSize + titleFontSize + screenManager.spacer()) / 4
    property int flagIconWidth: flagIconSize + screenManager.spacer()
    property int displayWidth: weightPanel.width - flagIconWidth - screenManager.spacer()
    property int weightWidth: displayWidth * 5.5 / (5.5 + 6.5 + 8.5)
    property int priceWidth: displayWidth * 6.5 / (5.5 + 6.5 + 8.5)
    property int amountWidth: displayWidth - weightWidth - priceWidth

    height: valueFontSize + titleFontSize + screenManager.spacer()
    Material.background: Material.color(Material.Grey, Material.Shade900)
    color: Material.background

    Connections // Slot for signal AppManager::showControlParam:
    {
        target: app
        function onShowControlParam(param, value)
        {
            //app.debugLog("@@@@@ weightPanel.onShowControlParam %1 %2".arg(param).arg(value));
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
            case 29: // WeightError or AutoPrint
                weightPanelIcon1.source = value
                break
            case 30: // TareFlag >T<
                weightPanelIcon3.source = value
                break
            case 31: // ZeroFlag >0<
                weightPanelIcon4.source = value
                break
            case 14: // WeightFixedFlag
                weightPanelIcon2.source = value
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
                MouseArea
                {
                    anchors.fill: parent
                    onClicked: app.onClicked(27);
                }

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

            Spacer { height: screenManager.spacer() / 2 }

            Text
            {
                id: weightPanelWeightTitle
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: weightWidth
                height: titleFontSize
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelWeightValue
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: weightWidth
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                color: Material.foreground
                MouseArea
                {
                    anchors.fill: parent
                    onClicked: app.onClicked2(20, 1);
                }
            }
        }

        Column
        {
            width: priceWidth
            height: parent.height
            spacing: 0
            padding: 0

            Spacer { height: screenManager.spacer() / 2 }

            Text
            {
                id: weightPanelPriceTitle
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: priceWidth
                height: titleFontSize
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelPriceValue
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: priceWidth
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                color: Material.foreground
                MouseArea
                {
                    anchors.fill: parent
                    onClicked: app.onClicked2(20, 2);
                }
            }
        }

        Column
        {
            width: amountWidth
            height: parent.height
            spacing: 0
            padding: 0

            Spacer { height: screenManager.spacer() / 2 }

            Text
            {
                id: weightPanelAmountTitle
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: amountWidth
                height: titleFontSize
                font { pointSize: titleFontSize }
                color: Material.color(Material.Grey, Material.Shade400)
            }

            Text
            {
                id: weightPanelAmountValue
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                width: amountWidth
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic' }
                //color: Material.foreground
                color: Material.color(Material.Orange, Material.Shade100)
                MouseArea
                {
                    anchors.fill: parent
                    onClicked: app.onClicked2(20, 3);
                }
            }
        }
    }
}


