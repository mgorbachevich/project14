import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Text
{
    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: Constants.margin
    anchors.rightMargin: Constants.margin
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font { pointSize: Constants.normalFontSize }
    color: Material.color(Material.BlueGrey, Material.Shade900)
    wrapMode: Text.WordWrap
    text: ""
}
