import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Text
{
    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: screenManager.spacer()
    anchors.rightMargin: screenManager.spacer()
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font { pointSize: screenManager.normalFontSize() }
    color: Material.color(Material.BlueGrey, Material.Shade900)
    wrapMode: Text.WordWrap
    text: ""
}
