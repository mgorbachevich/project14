import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    width: screenManager.buttonSize()
    height: screenManager.buttonSize()
    Layout.preferredWidth: width
    Layout.preferredHeight: height
    radius: screenManager.spacer()
    leftInset: screenManager.spacer() / 4
    topInset: screenManager.spacer() / 4
    rightInset: screenManager.spacer() / 4
    bottomInset: screenManager.spacer() / 4
    horizontalPadding: screenManager.spacer() / 4
    verticalPadding: screenManager.spacer() / 4
    flat: true
    icon.source: "../Icons/empty"
    Material.background: Material.color(Material.BlueGrey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    background: Rectangle
    {
        color: Material.color(Material.BlueGrey, Material.Shade200)
        radius: screenManager.spacer()
    }
}
