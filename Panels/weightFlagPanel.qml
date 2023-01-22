import QtQuick

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
        width: weightPanel.flagSize
        height: weightPanel.flagSize
        source: "../Icons/weight_48"
    }

    Image
    {
        id: productPanel0Icon
        anchors.horizontalCenter: parent.horizontalCenter
        width: weightPanel.flagSize
        height: weightPanel.flagSize
        source: "../Icons/0_48"
    }

    Image
    {
        id: productPanelTareIcon
        anchors.horizontalCenter: parent.horizontalCenter
        width: weightPanel.flagSize
        height: weightPanel.flagSize
        source: "../Icons/tare_48"
    }
}
