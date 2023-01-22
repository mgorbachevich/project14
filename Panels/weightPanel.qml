import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants

Rectangle
{
    Material.theme: Material.Dark
    Material.background: Material.color(Material.BlueGrey, Material.Shade900)
    Material.foreground: Material.color(Material.Grey, Material.Shade50)

    id:  weightPanel
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

    Connections // Slot for signal AppManager::showWeight:
    {
        target: app
        function onShowWeight(value) { weightPanelWeightValue.text = value }
    }

    Connections // Slot for signal AppManager::showPrice:
    {
        target: app
        function onShowPrice(value) { weightPanelPriceValue.text = value  }
    }

    Connections // Slot for signal AppManager::showAmount:
    {
        target: app
        function onShowAmount(value) { weightPanelAmountValue.text = value }
    }

    Connections // Slot for signal AppManager::showWeightColor:
    {
        target: app
        function onShowWeightColor(value) { weightPanelWeightValue.color = value }
    }

    Connections // Slot for signal AppManager::showPriceColor:
    {
        target: app
        function onShowPriceColor(value) { weightPanelPriceValue.color = value  }
    }

    Connections // Slot for signal AppManager::showAmountColor:
    {
        target: app
        function onShowAmountColor(value) { weightPanelAmountValue.color = value }
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
                    id: productPanelWeightIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/weight_48"
                }

                Image
                {
                    id: productPanel0Icon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/0_48"
                }

                Image
                {
                    id: productPanelTareIcon
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: flagSize
                    height: flagSize
                    source: "../Icons/tare_48"
                }
            }
        }

        Column
        {
            width: weightWidth
            height: parent.height
            spacing: 0
            padding: 0

            Label
            {
                id: weightPanelWeightTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
                text: qsTr("МАССА, кг")
            }

            Label
            {
                id: weightPanelWeightValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
                text: ""
            }
        }

        Column
        {
            width: priceWidth
            height: parent.height
            spacing: 0
            padding: 0

            Label
            {
                id: weightPanelPriceTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
                text: qsTr("ЦЕНА, руб")
            }

            Label
            {
                id: weightPanelPriceValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
                text: ""
            }
        }

        Column
        {
            width: amountWidth
            height: parent.height
            spacing: 0
            padding: 0

            Label
            {
                id: weightPanelAmountTitle
                anchors.horizontalCenter: parent.horizontalCenter
                height: titleFontSize
                font { pointSize: titleFontSize; family:'Roboto'; styleName:'Regular' }
                color: Material.color(Material.Grey, Material.Shade400)
                text: qsTr("СТОИМОСТЬ, руб")
            }

            Label
            {
                id: weightPanelAmountValue
                anchors.horizontalCenter: parent.horizontalCenter
                height: valueFontSize
                font { pointSize: valueFontSize; family:'League Gothic'; styleName:'Regular' }
                color: Material.foreground
                text: ""
            }
        }
    }
}


