import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    width: screenManager.flagSize()
    height: screenManager.flagSize()
    Layout.preferredWidth: width
    Layout.preferredHeight: height
    radius: width / 2
    leftInset: 0
    topInset: 0
    rightInset: 0
    bottomInset: 0
    horizontalPadding: 0
    verticalPadding: 0
    flat: true
    focus: false
    icon.source: "../Icons/empty"
    icon.width: width
    icon.height: height
    Material.background: "transparent"
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    background: Rectangle
    {
        color: "transparent"
        radius: screenManager.flagSize() / 2
    }
}
